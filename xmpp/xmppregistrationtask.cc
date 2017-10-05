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

#include "talk/xmpp/xmppregistrationtask.h"

#include <string>
#include <vector>

#include "talk/base/base64.h"
#include "talk/base/common.h"
#include "talk/xmllite/xmlelement.h"
#include "talk/xmpp/constants.h"
#include "talk/xmpp/jid.h"
#include "talk/xmpp/saslmechanism.h"
#include "talk/xmpp/xmppengineimpl.h"
#include <utils/Log.h>

using talk_base::ConstantLabel;

namespace buzz {

#ifdef _DEBUG
const ConstantLabel XmppRegistrationTask::REGISTRATIONTASK_STATES[] = {
  KLABEL(REGISTRATIONSTATE_INIT),
  KLABEL(REGISTRATIONSTATE_STREAMSTART_SENT),
  KLABEL(REGISTRATIONSTATE_STARTED_XMPP),
  KLABEL(REGISTRATIONSTATE_TLS_INIT),
  KLABEL(REGISTRATIONSTATE_TLS_REQUESTED),
  KLABEL(REGISTRATIONSTATE_FETCHFIELDS_SENT),
  KLABEL(REGISTRATIONSTATE_REGISTER_SENT),
  KLABEL(REGISTRATIONSTATE_DONE),
  LASTLABEL
};
#endif  // _DEBUG
XmppRegistrationTask::XmppRegistrationTask(XmppEngine * pctx, talk_base::CryptString pass) :
  state_(REGISTRATIONSTATE_INIT),
  pelStanza_(NULL),
  isStart_(false),
  iqId_(STR_EMPTY),
  pelFeatures_(NULL),
  fullJid_(STR_EMPTY),
  streamId_(STR_EMPTY),
  pass_(pass),
  pvecQueuedStanzas_(new std::vector<XmlElement *>()) {
      pctx_ = (XmppEngineImpl*)pctx;
}

XmppRegistrationTask::~XmppRegistrationTask() {
  for (size_t i = 0; i < pvecQueuedStanzas_->size(); i += 1)
    delete (*pvecQueuedStanzas_)[i];
}

void
XmppRegistrationTask::IncomingStanza(const XmlElement *element, bool isStart) {
  pelStanza_ = element;
  isStart_ = isStart;
  Advance();
  pelStanza_ = NULL;
  isStart_ = false;
}

const XmlElement *
XmppRegistrationTask::NextStanza() {
  const XmlElement * result = pelStanza_;
  pelStanza_ = NULL;
  return result;
}

bool
XmppRegistrationTask::Advance() {

  for (;;) {

    const XmlElement * element = NULL;

    // LOGD("%s:%d advance, current state: %d\n", __func__, __LINE__, state_);
#if _DEBUG
    BLOG(LS_VERBOSE) << "XmppRegistrationTask::Advance - "
      << talk_base::ErrorName(state_, REGISTRATIONTASK_STATES);
#endif  // _DEBUG

    switch (state_) {

      case REGISTRATIONSTATE_INIT: {
        pctx_->RaiseReset();
        pelFeatures_.reset(NULL);

        // The proper domain to verify against is the real underlying
        // domain - i.e., the domain that owns the JID.  Our XmppEngineImpl
        // also allows matching against a proxy domain instead, if it is told
        // to do so - see the implementation of XmppEngineImpl::StartTls and
        // XmppEngine::SetTlsServerDomain to see how you can use that feature
        pctx_->InternalSendStart(pctx_->user_jid_.domain());
        state_ = REGISTRATIONSTATE_STREAMSTART_SENT;
        break;
      }

      case REGISTRATIONSTATE_STREAMSTART_SENT: {
        if (NULL == (element = NextStanza()))
          return true;

        if (!isStart_ || !HandleStartStream(element))
          return Failure(XmppEngine::ERROR_VERSION);

        state_ = REGISTRATIONSTATE_STARTED_XMPP;
        return true;
      }

      case REGISTRATIONSTATE_STARTED_XMPP: {
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
          state_ = REGISTRATIONSTATE_TLS_INIT;
          continue;
        }

        // <iq id='uid-a7563d38-00000001' type='get' xmlns='jabber:client'>
        //    <query xmlns='jabber:iq:register'/>
        // </iq>
        talk_base::scoped_ptr<XmlElement> fetch_fields(new XmlElement(QN_IQ));
        fetch_fields->SetAttr(QN_TYPE, STR_GET);
        fetch_fields->SetAttr(QN_ID, pctx_->NextId());
        XmlElement *query = new XmlElement(QN_REGISTER_QUERY, true);
        fetch_fields->AddElement(query);
        pctx_->InternalSendStanza(fetch_fields.get());
        // LOGFL("<< SendFetchFields:%s", fetch_fields->Str().c_str());
        state_ = REGISTRATIONSTATE_FETCHFIELDS_SENT;
        return true;
      }

      case REGISTRATIONSTATE_TLS_INIT: {
        const XmlElement * pelTls = GetFeature(QN_TLS_STARTTLS);
        if (!pelTls)
          return Failure(XmppEngine::ERROR_TLS);

        XmlElement el(QN_TLS_STARTTLS, true);
        pctx_->InternalSendStanza(&el);
        state_ = REGISTRATIONSTATE_TLS_REQUESTED;
        continue;
      }

      case REGISTRATIONSTATE_TLS_REQUESTED: {
        if (NULL == (element = NextStanza()))
          return true;
        if (element->Name() != QN_TLS_PROCEED)
          return Failure(XmppEngine::ERROR_TLS);

        // The proper domain to verify against is the real underlying
        // domain - i.e., the domain that owns the JID.  Our XmppEngineImpl
        // also allows matching against a proxy domain instead, if it is told
        // to do so - see the implementation of XmppEngineImpl::StartTls and
        // XmppEngine::SetTlsServerDomain to see how you can use that feature
        pctx_->StartTls(pctx_->user_jid_.domain());
        pctx_->tls_option_ = buzz::TLS_ENABLED;
        state_ = REGISTRATIONSTATE_INIT;
        continue;
      }

      case REGISTRATIONSTATE_FETCHFIELDS_SENT: {
        // verify fields
        // send register iq
        // <iq id='uid-a7563d38-00000002' type='set' xmlns='jabber:client'><query xmlns='jabber:iq:register'><username>ray333</username><password>ray</password><name/><email/></query></iq>
        //
        talk_base::scoped_ptr<XmlElement> registration(new XmlElement(QN_IQ));
        registration->SetAttr(QN_TYPE, STR_SET);
        registration->SetAttr(QN_ID, pctx_->NextId());
        XmlElement *query = new XmlElement(QN_REGISTER_QUERY, true);
        XmlElement *username = new XmlElement(QN_REGISTER_USERNAME, true);
        username->SetBodyText(pctx_->user_jid_.node());
        XmlElement *password = new XmlElement(QN_REGISTER_PASSWORD, true);
        talk_base::scoped_ptr<char> pass(new char[pass_.GetLength()+1]);
        pass_.CopyTo(pass.get(), true);
        password->SetBodyText(pass.get());
        XmlElement *email = new XmlElement(QN_REGISTER_EMAIL, true);
        query->AddElement(username);
        query->AddElement(password);
        query->AddElement(email);
        registration->AddElement(query);
        pctx_->InternalSendStanza(registration.get());
        // LOGFL("<< SendRegistration:%s", registration->Str().c_str());
        state_ = REGISTRATIONSTATE_REGISTER_SENT;
        return true;
      }

      case REGISTRATIONSTATE_REGISTER_SENT: {
        // verify result
        // <iq type='result' id='reg2' />
        //
        // <cli:iq type="error" id="1" to="rmd-ubuntu/a0c99723" xmlns:cli="jabber:client">
        //     <query xmlns="jabber:iq:register"><username>ray</username><password>ray</password><email/></query>
        //     <cli:error code="409" type="cancel"><conflict xmlns="urn:ietf:params:xml:ns:xmpp-stanzas"/></cli:error>
        // </cli:iq>
        //
        if (NULL == (element = NextStanza()))
          return Failure(XmppEngine::ERROR_REGISTER);

        const XmlElement* error_xml_elem = element->FirstNamed(QN_ERROR);
        if (error_xml_elem != NULL) {//QN_STANZA_CONFLICT
            if(error_xml_elem->FirstNamed(QN_STANZA_CONFLICT)) {
                // LOGFL("error_xml_elem: %s", error_xml_elem->Str().c_str());
                return Failure(XmppEngine::ERROR_REGISTER_CONFLICT);
            }
        }

        if (element->HasAttr(QN_TYPE) && element->Attr(QN_TYPE) == STR_RESULT)
          state_ = REGISTRATIONSTATE_DONE;
        else
          return Failure(XmppEngine::ERROR_REGISTER);
        pctx_->SignalSessionClosed();
        continue;
      }

      case REGISTRATIONSTATE_DONE:
        return false;
    }
  }
}

bool XmppRegistrationTask::HandleStartStream(const XmlElement *element) {
  if (element->Name() != QN_STREAM_STREAM)
    return false;

  if (element->Attr(QN_XMLNS) != "jabber:client")
    return false;

  if (element->Attr(QN_VERSION) != "1.0")
    return false;

  if (!element->HasAttr(QN_ID))
    return false;

  streamId_ = element->Attr(QN_ID);

  return true;
}

bool
XmppRegistrationTask::HandleFeatures(const XmlElement *element) {
  if (element->Name() != QN_STREAM_FEATURES)
    return false;

  pelFeatures_.reset(new XmlElement(*element));
  return true;
}

const XmlElement *
XmppRegistrationTask::GetFeature(const QName & name) {
  return pelFeatures_->FirstNamed(name);
}

bool XmppRegistrationTask::Failure(XmppEngine::Error reason) {
  state_ = REGISTRATIONSTATE_DONE;
  pctx_->SignalError(reason, 0);
  return false;
}

void
XmppRegistrationTask::OutgoingStanza(const XmlElement * element) {
  XmlElement * pelCopy = new XmlElement(*element);
  pvecQueuedStanzas_->push_back(pelCopy);
}

void
XmppRegistrationTask::FlushQueuedStanzas() {
  for (size_t i = 0; i < pvecQueuedStanzas_->size(); i += 1) {
    pctx_->InternalSendStanza((*pvecQueuedStanzas_)[i]);
    delete (*pvecQueuedStanzas_)[i];
  }
  pvecQueuedStanzas_->clear();
}

}
