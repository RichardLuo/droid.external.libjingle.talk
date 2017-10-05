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

#include "talk/xmpp/xmpplocallogintask.h"

#include <string>
#include <vector>

#include "talk/base/base64.h"
#include "talk/base/common.h"
#include "talk/xmllite/xmlelement.h"
#include "talk/xmpp/constants.h"
#include "talk/xmpp/jid.h"
#include "talk/xmpp/saslmechanism.h"
#include "talk/xmpp/xmpplocalengine.h"
#include <stdio.h>


using talk_base::ConstantLabel;

namespace buzz {

#ifdef _DEBUG
const ConstantLabel XmppLocalLoginTask::LOGINTASK_STATES[] = {
  KLABEL(LOGINSTATE_INIT),
  KLABEL(LOGINSTATE_STREAMSTART_SENT),
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
XmppLocalLoginTask::XmppLocalLoginTask(XmppLocalEngine * pctx, bool passive) :
  pctx_(pctx),
  authNeeded_(true),
  allowNonGoogleLogin_(true),
  state_(LOGINSTATE_INIT),
  pelStanza_(NULL),
  isStart_(false),
  iqId_(STR_EMPTY),
  pelFeatures_(NULL),
  fullJid_(STR_EMPTY),
  streamId_(STR_EMPTY),
  passive_(passive),
  pvecQueuedStanzas_(new std::vector<XmlElement *>()),
  sasl_mech_(NULL) {
}

XmppLocalLoginTask::~XmppLocalLoginTask() {
  for (size_t i = 0; i < pvecQueuedStanzas_->size(); i += 1)
    delete (*pvecQueuedStanzas_)[i];
}

void
XmppLocalLoginTask::IncomingStanza(const XmlElement *element, bool isStart) {
  pelStanza_ = element;
  isStart_ = isStart;
  Advance();
  pelStanza_ = NULL;
  isStart_ = false;
}

const XmlElement *
XmppLocalLoginTask::NextStanza() {
  const XmlElement * result = pelStanza_;
  pelStanza_ = NULL;
  return result;
}

bool
XmppLocalLoginTask::Advance() {

  for (;;) {

    const XmlElement * element = NULL;

#if _DEBUG
    BLOG(LS_VERBOSE) << "XmppLocalLoginTask::Advance - "
      << talk_base::ErrorName(state_, LOGINTASK_STATES);
#endif  // _DEBUG

    switch (state_) {

      case LOGINSTATE_INIT: {
        pctx_->RaiseReset();
        pelFeatures_.reset(NULL);

        if (!passive_) {
        // The proper domain to verify against is the real underlying
        // domain - i.e., the domain that owns the JID.  Our XmppLocalEngine
        // also allows matching against a proxy domain instead, if it is told
        // to do so - see the implementation of XmppLocalEngine::StartTls and
        // XmppEngine::SetTlsServerDomain to see how you can use that feature
          pctx_->InternalSendStart();
        }
        state_ = LOGINSTATE_STREAMSTART_SENT;
        break;
      }

      case LOGINSTATE_STREAMSTART_SENT: {
        if (NULL == (element = NextStanza()))
          return true;

        if (!isStart_ || !HandleStartStream(element)) {
          fprintf(stderr, "isStart_:%d ERR-element:%s\n", isStart_, element->Str().c_str());
          return Failure(XmppEngine::ERROR_VERSION);
        }

        fprintf(stderr, "OK, got start-element:[%s]\n", element->Str().c_str());

        if (passive_) {
            fprintf(stderr, "got peer jid:%s, and mine preseted peer is:%s \n",
                    peer_jid_.Str().c_str(), pctx_->GetPeerUser().Str().c_str());
            pctx_->SetPeerUser(peer_jid_);
            fprintf(stderr, "got peer jid:%s, and mine preseted peer is:%s \n",
                    peer_jid_.Str().c_str(), pctx_->GetPeerUser().Str().c_str());
            // TODO: check the user_jid_
           pctx_->InternalSendStart();
        }

        pctx_->SignalBound(fullJid_);
        state_ = LOGINSTATE_DONE;
        fprintf(stderr, "got LOGINSTATE_DONE, IsDone():%d \n", IsDone());
        return true;
      }

      case LOGINSTATE_DONE:
        FlushQueuedStanzas();
        return false;

      default:
        return false;
    }
  }
}

bool
XmppLocalLoginTask::HandleStartStream(const XmlElement *element) {

  if (element->Name() != QN_STREAM_STREAM) {
    printf("name err:%s\n", element->Str().c_str());
    return false;
  }

  if (element->Attr(QN_XMLNS) != "jabber:client") {
    printf("ns err \n");
    return false;
  }

  if (element->Attr(QN_VERSION) != "1.0") {
    printf("version err! \n");
    return false;
  }

  peer_jid_ = Jid(element->Attr(QN_FROM));
  user_jid_ = Jid(element->Attr(QN_TO));

  if (peer_jid_.IsValid() && user_jid_.IsValid()) {
    fprintf(stderr, "peer:%s mine:%s \n", peer_jid_.Str().c_str(), user_jid_.Str().c_str());
    return true;
  } else {
    fprintf(stderr, "peer_jid_:%d user_jid_:%d \n", peer_jid_.IsValid(), user_jid_.IsValid());
    return false;
  }

  return false;
}

bool
XmppLocalLoginTask::HandleFeatures(const XmlElement *element) {
  if (element->Name() != QN_STREAM_FEATURES)
    return false;

  pelFeatures_.reset(new XmlElement(*element));
  return true;
}

const XmlElement *
XmppLocalLoginTask::GetFeature(const QName & name) {
  return pelFeatures_->FirstNamed(name);
}

bool
XmppLocalLoginTask::Failure(XmppEngine::Error reason) {
  state_ = LOGINSTATE_DONE;
  pctx_->SignalError(reason, 0);
  return false;
}

void
XmppLocalLoginTask::OutgoingStanza(const XmlElement * element) {
  XmlElement * pelCopy = new XmlElement(*element);
  pvecQueuedStanzas_->push_back(pelCopy);
}

void
XmppLocalLoginTask::FlushQueuedStanzas() {
  for (size_t i = 0; i < pvecQueuedStanzas_->size(); i += 1) {
    pctx_->InternalSendStanza((*pvecQueuedStanzas_)[i]);
    delete (*pvecQueuedStanzas_)[i];
  }
  pvecQueuedStanzas_->clear();
}

}
