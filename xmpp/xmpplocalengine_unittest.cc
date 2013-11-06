// Copyright 2004 Google Inc. All Rights Reserved
// Author: David Bau

#include <string>
#include <sstream>
#include <iostream>
#include "talk/base/common.h"
#include "talk/base/gunit.h"
#include "talk/xmllite/xmlelement.h"
#include "talk/xmpp/constants.h"
#include "talk/xmpp/util_unittest.h"
#include "talk/xmpp/saslplainmechanism.h"
#include "talk/xmpp/plainsaslhandler.h"
#include "talk/xmpp/xmpplocalengine.h"

using buzz::Jid;
using buzz::QName;
using buzz::XmlElement;
using buzz::XmppEngine;
using buzz::XmppIqCookie;
using buzz::XmppIqHandler;
using buzz::XmppTestHandler;
using buzz::XmppLocalEngine;
using buzz::QN_ID;
using buzz::QN_IQ;
using buzz::QN_TYPE;
using buzz::QN_ROSTER_QUERY;
using buzz::XMPP_RETURN_OK;
using buzz::XMPP_RETURN_BADARGUMENT;

// XmppLocalEngineTestIqHandler
//    This class grabs the response to an IQ stanza and stores it in a string.
class XmppLocalEngineTestIqHandler : public XmppIqHandler {
 public:
  virtual void IqResponse(XmppIqCookie, const XmlElement * stanza) {
    ss_ << stanza->Str();
  }

  std::string IqResponseActivity() {
    std::string result = ss_.str();
    ss_.str("");
    return result;
  }

 private:
  std::stringstream ss_;
};

class XmppLocalEngineTest : public testing::Test {
 public:
  XmppEngine* engine() { return engine_.get(); }
  XmppTestHandler* handler() { return handler_.get(); }
  virtual void SetUp() {
    engine_.reset(new XmppLocalEngine(false));
    handler_.reset(new XmppTestHandler(engine_.get()));

    Jid user_jid("kitty@localhost");
    Jid peer_jid("fish@localhost");
    talk_base::InsecureCryptStringImpl pass;
    pass.password() = "miao";
    engine_->SetSessionHandler(handler_.get());
    engine_->SetOutputHandler(handler_.get());
    engine_->AddStanzaHandler(handler_.get());
    engine_->SetUser(user_jid);
    engine_->SetPeerUser(peer_jid);
    engine_->SetSaslHandler(
        new buzz::PlainSaslHandler(user_jid, talk_base::CryptString(pass), true));
  }
  virtual void TearDown() {
    handler_.reset();
    engine_.reset();
  }
  void RunLogin();

  void RunLocalLogin();

 private:
  talk_base::scoped_ptr<XmppEngine> engine_;
  talk_base::scoped_ptr<XmppTestHandler> handler_;
};

void XmppLocalEngineTest::RunLogin() {
}

void XmppLocalEngineTest::RunLocalLogin() {
    std::string input;
    // Connect
    EXPECT_EQ(XmppEngine::STATE_START, engine()->GetState());
    engine()->Connect();
    EXPECT_EQ(XmppEngine::STATE_OPENING, engine()->GetState());

    EXPECT_EQ("[OPENING]", handler_->SessionActivity());

    EXPECT_EQ("<stream:stream xmlns=\"jabber:client\" "
              "xmlns:stream=\"http://etherx.jabber.org/streams\"  "
              "from=\"kitty@localhost\"  "
              "to=\"fish@localhost\" "
              "version=\"1.0\"> \r\n", handler_->OutputActivity());


    input = "<stream:stream xmlns=\"jabber:client\" "
            "xmlns:stream=\"http://etherx.jabber.org/streams\"  "
            "from=\"fish@localhost\" "
            "to=\"kitty@localhost\"  "
            "version=\"1.0\">";

    engine()->HandleInput(input.c_str(), input.length());

    EXPECT_EQ("", handler_->OutputActivity());
    EXPECT_EQ("[OPEN]", handler()->SessionActivity());

    input = "<message from=\"fish@localhost\" to=\"kitty@localhost\">"
            "<body> hello kitty </body>"
            "</message>";

    engine()->HandleInput(input.c_str(), input.length());
    EXPECT_EQ("", handler()->StanzaActivity());

    input = "<iq from=\"fish@localhost\" "
            "id=\"i3s91xc4\" "
            "to=\"kitty@localhost\" "
            "type=\"set\"> "
            "<block xmlns=\"urn:xmpp:blocking\"> "
            "<item jid=\"spammers.lit\"/> "
            "</block> "
            "</iq>";
    engine()->HandleInput(input.c_str(), input.length());
    EXPECT_EQ("", handler()->StanzaActivity());

}

// TestSuccessfulLogin()
//    This function simply tests to see if a login works.  This includes
//    encryption and authentication
TEST_F(XmppLocalEngineTest, TestLogin) {
  RunLocalLogin();
  engine()->Disconnect();
  EXPECT_EQ("</stream:stream>[CLOSED]", handler()->OutputActivity());
  EXPECT_EQ("[CLOSED]", handler()->SessionActivity());
  EXPECT_EQ("", handler()->StanzaActivity());
}

