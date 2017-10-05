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

#ifndef TALK_XMPP_REGISTRATIONTASK_H_
#define TALK_XMPP_REGISTRATIONTASK_H_

#include <string>
#include <vector>

#include "talk/base/logging.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/cryptstring.h"
#include "talk/xmpp/jid.h"
#include "talk/xmpp/xmppengine.h"
#include "talk/xmpp/xmpplogininterface.h"

namespace buzz {

class XmlElement;
class XmppEngineImpl;
class SaslMechanism;

class XmppRegistrationTask : public XmppLoginInterface {

public:
  XmppRegistrationTask(XmppEngine *pctx, talk_base::CryptString pass);
  ~XmppRegistrationTask();

  bool IsDone() const {
      return state_ == REGISTRATIONSTATE_DONE;
  }

  void IncomingStanza(const XmlElement * element, bool isStart);
  void OutgoingStanza(const XmlElement *element);

private:
  enum RegistrationTaskState {
    REGISTRATIONSTATE_INIT = 0,
    REGISTRATIONSTATE_STREAMSTART_SENT,
    REGISTRATIONSTATE_STARTED_XMPP,
    REGISTRATIONSTATE_TLS_INIT,
    REGISTRATIONSTATE_TLS_REQUESTED,
    REGISTRATIONSTATE_FETCHFIELDS_SENT,
    REGISTRATIONSTATE_REGISTER_SENT,
    REGISTRATIONSTATE_DONE,
  };

  const XmlElement * NextStanza();
  bool Advance();
  bool HandleStartStream(const XmlElement * element);
  bool HandleFeatures(const XmlElement * element);
  const XmlElement * GetFeature(const QName & name);
  bool Failure(XmppEngine::Error reason);
  void FlushQueuedStanzas();

  XmppEngineImpl * pctx_;
  RegistrationTaskState state_;
  const XmlElement * pelStanza_;
  bool isStart_;
  std::string iqId_;
  talk_base::scoped_ptr<XmlElement> pelFeatures_;
  Jid fullJid_;
  std::string streamId_;
  talk_base::CryptString pass_;
  talk_base::scoped_ptr<std::vector<XmlElement *> > pvecQueuedStanzas_;

#ifdef _DEBUG
  static const talk_base::ConstantLabel REGISTRATIONTASK_STATES[];
#endif  // _DEBUG
};

}

#endif  //  TALK_XMPP_REGISTRATIONTASK_H_
