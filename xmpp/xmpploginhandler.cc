/*
 * libjingle
 * Copyright 2004--2005, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "talk/xmpp/xmpploginhandler.h"

#include <string>
#include <vector>

#include "talk/base/thread.h"
#include "talk/base/base64.h"
#include "talk/base/common.h"
#include "talk/xmllite/xmlelement.h"
#include "talk/xmpp/constants.h"
#include "talk/xmpp/jid.h"
#include "talk/xmpp/saslmechanism.h"
#include "talk/xmpp/xmppengineimpl.h"

//#define LOG_NDEBUG 0
#include <utils/Log.h>
#include <utils/Errors.h>

using talk_base::ConstantLabel;

namespace buzz {

#ifdef _DEBUG
const ConstantLabel XmppLoginHandler::LOGINTASK_STATES[] = {
    KLABEL(LOGINSTATE_INIT),
    KLABEL(LOGINSTATE_LISTEN_STREAMSTART),
    KLABEL(LOGINSTATE_STARTED_XMPP),
    KLABEL(LOGINSTATE_TLS_INIT),
    KLABEL(LOGINSTATE_AUTH_INIT),
    KLABEL(LOGINSTATE_BIND_INIT),
    KLABEL(LOGINSTATE_TLS_REQUESTED),
    KLABEL(LOGINSTATE_SASL_RUNNING),
    KLABEL(LOGINSTATE_BIND_REQUESTED),
    KLABEL(LOGINSTATE_SESSION_REQUESTED),
    KLABEL(LOGINSTATE_DONE),
    LASTLABEL
};
#endif  // _DEBUG
XmppLoginHandler::XmppLoginHandler(XmppEngineImpl * pctx) :
        pctx_(pctx),
        authNeeded_(false),
        authFinished_(false),
        tlsHandshakeFinished_(false),
        allowNonGoogleLogin_(true),
        state_(LOGINSTATE_INIT),
        pelStanza_(NULL),
        isStart_(false),
        iqId_(STR_EMPTY),
        pelFeatures_(NULL),
        fullJid_(STR_EMPTY),
        streamId_(STR_EMPTY),
        pvecQueuedStanzas_(new std::vector<XmlElement *>()),
        sasl_mech_(NULL) {
}

XmppLoginHandler::~XmppLoginHandler() {
    for (size_t i = 0; i < pvecQueuedStanzas_->size(); i += 1)
        delete (*pvecQueuedStanzas_)[i];
}

void
XmppLoginHandler::IncomingStanza(const XmlElement *element, bool isStart) {
    pelStanza_ = element;
    isStart_ = isStart;
    Advance();
    pelStanza_ = NULL;
    isStart_ = false;
}

const XmlElement *
XmppLoginHandler::NextStanza() {
    const XmlElement * result = pelStanza_;
    pelStanza_ = NULL;
    return result;
}

bool
XmppLoginHandler::Advance() {

    for (;;) {

        const XmlElement * element = NULL;

#if _DEBUG
        LOG(LS_VERBOSE) << "XmppLoginHandler::Advance - "
                        << talk_base::ErrorName(state_, LOGINTASK_STATES);
#endif  // _DEBUG

        switch (state_) {

            case LOGINSTATE_INIT: {
                pctx_->RaiseReset();
                pelFeatures_.reset(NULL);

                // The proper domain to verify against is the real underlying
                // domain - i.e., the domain that owns the JID.  Our XmppEngineImpl
                // also allows matching against a proxy domain instead, if it is told
                // to do so - see the implementation of XmppEngineImpl::StartTls and
                // XmppEngine::SetTlsServerDomain to see how you can use that feature
                // pctx_->InternalSendStart(pctx_->user_jid_.domain());
                state_ = LOGINSTATE_LISTEN_STREAMSTART;
                continue;
            }
            case LOGINSTATE_LISTEN_STREAMSTART: {
                if (NULL == (element = NextStanza()))
                    return true;

                LOGFL("got stream:stream:%s", element->Str().c_str());
                
                if (!isStart_ || !HandleStartStream(element))
                    return Failure(XmppEngine::ERROR_VERSION);

                pctx_->InternalSendStartResponse();
                state_ = LOGINSTATE_STREAMSTART_RSP_SENT;
                continue;
            }

            case LOGINSTATE_STREAMSTART_RSP_SENT: {
                pctx_->InternalSendFeatures(tlsHandshakeFinished_, authFinished_);
                if (tlsHandshakeFinished_ && authFinished_) {
                    pctx_->SignalSessionOpened();
                    FlushQueuedStanzas();
                    state_ = LOGINSTATE_DONE;
                } else {
                    state_ = LOGINSTATE_XMPP_FEATURES_SENT;
                }
                return true;
            }

            case LOGINSTATE_XMPP_FEATURES_SENT: {
                if (NULL == (element = NextStanza()))
                    return true;

                if (element->Name() == QN_SASL_AUTH) {
                    if (pctx_->ProcessSaslAuthStanza(element) < 0) {
                        LOGE("Invalid sasl-auth xml:%s", element->Str().c_str());
                        return Failure(XmppEngine::ERROR_XML);
                    }
                    authFinished_ = true;
                    state_ = LOGINSTATE_INIT;
                    LOGFL("auth-stanza processed ok!");
                    continue;
                }

                // <proceed xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>
                if (element->Name() == QN_TLS_STARTTLS) {
                    if (element->Attr(QN_XMLNS) != NS_TLS) {
                        LOGE("Invalid start-tls-xml:%s", element->Str().c_str());
                        return Failure(XmppEngine::ERROR_XML);
                    }
                    XmlElement stanza(QN_TLS_PROCEED, true);
                    pctx_->InternalSendStanza(&stanza);
                    state_ = LOGINSTATE_TLS_INIT;
                    LOGFL("tls-processed ok!");
                    talk_base::Thread::Current()->Post(pctx_);
                    return true;
                }
            }

            case LOGINSTATE_STARTED_XMPP: {
                if (NULL == (element = NextStanza()))
                    return true;

                if (!HandleFeatures(element))
                    return Failure(XmppEngine::ERROR_VERSION);

                bool tls_present = (GetFeature(QN_TLS_STARTTLS) != NULL);
                // Error if TLS required but not present.
                if (pctx_->tls_option_ == buzz::TLS_REQUIRED && !tls_present) {
                    return Failure(XmppEngine::ERROR_TLS);
                }
                // Use TLS if required or enabled, and also available
                if ((pctx_->tls_option_ == buzz::TLS_REQUIRED ||
                     pctx_->tls_option_ == buzz::TLS_ENABLED) && tls_present) {
                    state_ = LOGINSTATE_TLS_INIT;
                    continue;
                }

                if (authNeeded_) {
                    state_ = LOGINSTATE_AUTH_INIT;
                    continue;
                }

                state_ = LOGINSTATE_BIND_INIT;
                continue;
            }

                // case LOGINSTATE_TLS_INIT: {
                //     // const XmlElement * pelTls = GetFeature(QN_TLS_STARTTLS);
                //     // if (!pelTls)
                //     //     return Failure(XmppEngine::ERROR_TLS);
                //     XmlElement el(QN_TLS_STARTTLS, true);
                //     pctx_->InternalSendStanza(&el);
                //     state_ = LOGINSTATE_TLS_REQUESTED;
                //     continue;
                // }

            case LOGINSTATE_TLS_INIT: {
                // The proper domain to verify against is the real underlying
                // domain - i.e., the domain that owns the JID.  Our XmppEngineImpl
                // also allows matching against a proxy domain instead, if it is told
                // to do so - see the implementation of XmppEngineImpl::StartTls and
                // XmppEngine::SetTlsServerDomain to see how you can use that feature
                pctx_->StartTls(pctx_->user_jid_.domain());
                pctx_->tls_option_ = buzz::TLS_ENABLED;
                tlsHandshakeFinished_ = true;
                state_ = LOGINSTATE_INIT;
                continue;
            }

            case LOGINSTATE_AUTH_INIT: {
                const XmlElement * pelSaslAuth = GetFeature(QN_SASL_MECHANISMS);
                if (!pelSaslAuth) {
                    return Failure(XmppEngine::ERROR_AUTH);
                }

                // Collect together the SASL auth mechanisms presented by the server
                std::vector<std::string> mechanisms;
                for (const XmlElement * pelMech =
                             pelSaslAuth->FirstNamed(QN_SASL_MECHANISM);
                     pelMech;
                     pelMech = pelMech->NextNamed(QN_SASL_MECHANISM)) {

                    mechanisms.push_back(pelMech->BodyText());
                }

                // Given all the mechanisms, choose the best
                std::string choice(pctx_->ChooseBestSaslMechanism(mechanisms, pctx_->IsEncrypted()));
                if (choice.empty()) {
                    return Failure(XmppEngine::ERROR_AUTH);
                }

                // No recognized auth mechanism - that's an error
                sasl_mech_.reset(pctx_->GetSaslMechanism(choice));
                if (!sasl_mech_) {
                    return Failure(XmppEngine::ERROR_AUTH);
                }

                // OK, let's start it.
                XmlElement * auth = sasl_mech_->StartSaslAuth();
                if (auth == NULL) {
                    return Failure(XmppEngine::ERROR_AUTH);
                }
                if (allowNonGoogleLogin_) {
                    // Setting the following two attributes is required to support
                    // non-google ids.

                    // Allow login with non-google id accounts.
                    auth->SetAttr(QN_GOOGLE_ALLOW_NON_GOOGLE_ID_XMPP_LOGIN, "true");

                    // Allow login with either the non-google id or the friendly email.
                    auth->SetAttr(QN_GOOGLE_AUTH_CLIENT_USES_FULL_BIND_RESULT, "true");
                }

                pctx_->InternalSendStanza(auth);
                delete auth;
                state_ = LOGINSTATE_SASL_RUNNING;
                continue;
            }

            case LOGINSTATE_SASL_RUNNING: {
                if (NULL == (element = NextStanza()))
                    return true;
                if (element->Name().Namespace() != NS_SASL)
                    return Failure(XmppEngine::ERROR_AUTH);
                if (element->Name() == QN_SASL_CHALLENGE) {
                    XmlElement * response = sasl_mech_->HandleSaslChallenge(element);
                    if (response == NULL) {
                        return Failure(XmppEngine::ERROR_AUTH);
                    }
                    pctx_->InternalSendStanza(response);
                    delete response;
                    state_ = LOGINSTATE_SASL_RUNNING;
                    continue;
                }
                if (element->Name() != QN_SASL_SUCCESS) {
                    return Failure(XmppEngine::ERROR_UNAUTHORIZED);
                }

                // Authenticated!
                authNeeded_ = false;
                state_ = LOGINSTATE_INIT;
                continue;
            }

            case LOGINSTATE_BIND_INIT: {
                const XmlElement * pelBindFeature = GetFeature(QN_BIND_BIND);
                const XmlElement * pelSessionFeature = GetFeature(QN_SESSION_SESSION);
                if (!pelBindFeature || !pelSessionFeature)
                    return Failure(XmppEngine::ERROR_BIND);

                XmlElement iq(QN_IQ);
                iq.AddAttr(QN_TYPE, "set");

                iqId_ = pctx_->NextId();
                iq.AddAttr(QN_ID, iqId_);
                iq.AddElement(new XmlElement(QN_BIND_BIND, true));

                if (pctx_->requested_resource_ != STR_EMPTY) {
                    iq.AddElement(new XmlElement(QN_BIND_RESOURCE), 1);
                    iq.AddText(pctx_->requested_resource_, 2);
                }
                pctx_->InternalSendStanza(&iq);
                state_ = LOGINSTATE_BIND_REQUESTED;
                continue;
            }

            case LOGINSTATE_BIND_REQUESTED: {
                if (NULL == (element = NextStanza()))
                    return true;

                if (element->Name() != QN_IQ || element->Attr(QN_ID) != iqId_ ||
                    element->Attr(QN_TYPE) == "get" || element->Attr(QN_TYPE) == "set")
                    return true;

                if (element->Attr(QN_TYPE) != "result" || element->FirstElement() == NULL ||
                    element->FirstElement()->Name() != QN_BIND_BIND)
                    return Failure(XmppEngine::ERROR_BIND);

                fullJid_ = Jid(element->FirstElement()->TextNamed(QN_BIND_JID));
                if (!fullJid_.IsFull()) {
                    return Failure(XmppEngine::ERROR_BIND);
                }

                // now request session
                XmlElement iq(QN_IQ);
                iq.AddAttr(QN_TYPE, "set");

                iqId_ = pctx_->NextId();
                iq.AddAttr(QN_ID, iqId_);
                iq.AddElement(new XmlElement(QN_SESSION_SESSION, true));
                pctx_->InternalSendStanza(&iq);

                state_ = LOGINSTATE_SESSION_REQUESTED;
                continue;
            }

            case LOGINSTATE_SESSION_REQUESTED: {
                if (NULL == (element = NextStanza()))
                    return true;
                if (element->Name() != QN_IQ || element->Attr(QN_ID) != iqId_ ||
                    element->Attr(QN_TYPE) == "get" || element->Attr(QN_TYPE) == "set")
                    return false;

                if (element->Attr(QN_TYPE) != "result")
                    return Failure(XmppEngine::ERROR_BIND);

                pctx_->SignalBound(fullJid_);
                FlushQueuedStanzas();
                state_ = LOGINSTATE_DONE;
                return true;
            }

            case LOGINSTATE_DONE:
                return false;
        }
    }
}

// <stream:stream 
//     to="localhost" 
//     xml:lang="*" 
//     version="1.0" 
//     xmlns:stream="http://etherx.jabber.org/streams" xmlns="jabber:client"/> 
bool XmppLoginHandler::HandleStartStream(const XmlElement *element) {

    LOGFL("got start-stream:%s \n", element->Str().c_str());

    if (element->Name() != QN_STREAM_STREAM)
        return false;

    if (element->Attr(QN_XMLNS) != "jabber:client")
        return false;

    if (element->Attr(QN_VERSION) != "1.0")
        return false;

    // if (!element->HasAttr(QN_ID))
    //   return false;
    // streamId_ = element->Attr(QN_ID);

    return true;
}

bool XmppLoginHandler::HandleFeatures(const XmlElement *element) {
    if (element->Name() != QN_STREAM_FEATURES)
        return false;

    pelFeatures_.reset(new XmlElement(*element));
    return true;
}

const XmlElement *
XmppLoginHandler::GetFeature(const QName & name) {
    return pelFeatures_->FirstNamed(name);
}

bool
XmppLoginHandler::Failure(XmppEngine::Error reason) {
    state_ = LOGINSTATE_DONE;
    pctx_->SignalError(reason, 0);
    return false;
}

void
XmppLoginHandler::OutgoingStanza(const XmlElement * element) {
    XmlElement * pelCopy = new XmlElement(*element);
    pvecQueuedStanzas_->push_back(pelCopy);
}

void
XmppLoginHandler::FlushQueuedStanzas() {
    for (size_t i = 0; i < pvecQueuedStanzas_->size(); i += 1) {
        pctx_->InternalSendStanza((*pvecQueuedStanzas_)[i]);
        delete (*pvecQueuedStanzas_)[i];
    }
    pvecQueuedStanzas_->clear();
}

}
