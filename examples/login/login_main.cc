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

#include <cstdio>
#include <iostream>

#include "talk/base/thread.h"
#include "talk/xmpp/constants.h"
#include "talk/xmpp/xmppclientsettings.h"
#include "talk/xmpp/xmppengine.h"
#include "talk/xmpp/xmppthread.h"


// TODO: Move this into Console.
static void Print(const char* chars) {
  printf("%s", chars);
  fflush(stdout);
}

int main(int argc, char **argv) {

    buzz::Jid jid;
    std::string username;

    if (username.empty()) {
        Print("JID: ");
        std::cin >> username;
    }
    if (username.find('@') == std::string::npos) {
        username.append("@localhost");
    }

    jid = buzz::Jid(username);
    if (!jid.IsValid() || jid.node() == "") {
        Print("Invalid JID. JIDs should be in the form user@domain\n");
        return 1;
    }

    talk_base::InsecureCryptStringImpl pass;
    if (pass.password().empty()) {
        Print("Password: ");
        std::cin >> pass.password();
        Print("\n");
    }

    // Decide on the connection settings.
    buzz::XmppClientSettings xcs;
    xcs.set_user(jid.node());
    xcs.set_resource("call");
    xcs.set_host(jid.domain());
    xcs.set_allow_plain(true);

    // Start xmpp on a different thread
    buzz::XmppThread thread;
    thread.Start();

    xcs.set_use_tls(buzz::TLS_DISABLED);
    xcs.set_server(talk_base::SocketAddress("talk.google.com", 5222));
    xcs.set_pass(talk_base::CryptString(pass));

    thread.Login(xcs);

    // Use main thread for console input
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "quit")
            break;
    }
    return 0;
}
