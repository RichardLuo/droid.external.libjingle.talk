/*
 * Copyright (C) 2013 The X-Live Project
 *
 * @author Richard Luo, cdominant7@gmail.com
 * 
 * @date   2013-06-29 14:46:22
 * 
 ****************************************************************** 
 */

#define LOG_TAG "echo_tcp_server"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>
#include <utils/Log.h>

#include "talk/base/flags.h"
#include "common/utils.h"

#include "talk/base/asynchttprequest.h"
#include "talk/base/socketaddress.h"


#include "talk/base/httpserver.h"
#include "talk/base/socketstream.h"
#include "talk/base/thread.h"

using namespace talk_base;

DEFINE_bool(help, false, "Prints this message");
DEFINE_int(port, 9999, "The port on which to listen.");

static const size_t kMaxConnections = (FD_SETSIZE - 2);


class EchoTcpServer : public sigslot::has_slots<> {

  public:

    int init(Thread* thread, const SocketAddress& addr) {
        mListenSocket.reset(thread->socketserver()->CreateAsyncSocket(addr.family(), SOCK_STREAM));
        LOG_IF_RETURN(mListenSocket->Bind(addr), "ERR: on Bind!");
        LOG_IF_RETURN(mListenSocket->Listen(5), "ERR: on Listen!");
        mListenSocket->SignalReadEvent.connect(this, &EchoTcpServer::OnAccept);
        LOGFL("<< SignalReadEvent.connect()!");
        return 0;
    }

    SocketAddress address() const { return mListenSocket->GetLocalAddress(); }

    void Close() const { mListenSocket->Close(); }

  private:

    void OnAccept(AsyncSocket* socket) {
        fprintf(stderr, "OnAccept! \n");
        fflush(0);
        AsyncSocket* new_socket = mListenSocket->Accept(NULL);
        if (new_socket) {
            new_socket->SignalReadEvent.connect(this, &EchoTcpServer::HandleReadEvent);
            new_socket->SignalCloseEvent.connect(this, &EchoTcpServer::HandleCloseEvent);
            mDataSockets.push_back(new_socket);

            // XmppEngine *engine = XmppEngine::Create();
            // // handler_.reset(new XmppTestHandler(engine_.get()));
            // Jid jid("kitty@localhost");
            // talk_base::InsecureCryptStringImpl pass;
            // pass.password() = "miao";
            // engine->SetSessionHandler(handler_.get());
            // engine->SetOutputHandler(handler_.get());
            // engine->AddStanzaHandler(handler_.get());
            // engine->SetUser(jid);
            // engine->SetSaslHandler(new buzz::PlainSaslHandler(jid, talk_base::CryptString(pass), true));

        }
    }

    void HandleReadEvent(AsyncSocket *sock) {
        uint8_t buf[4096];
        int len = sock->Recv(buf, sizeof(buf));
        if (len < 0) {
            // TODO: Do something better like forwarding the error to the user.
            if (!sock->IsBlocking()) {
                //LOG(LS_ERROR) << "Recv() returned error: " << sock->GetError();
            }
            return;
        }
        hexdump_info(buf, len, "got data:");

        std::string input =
                "<stream:stream id=\"a5f2d8c9\" version=\"1.0\" "
                "xmlns:stream=\"http://etherx.jabber.org/streams\" "
                "xmlns=\"jabber:client\">";
        sock->Send(input.c_str(), input.length());
    }

    void HandleCloseEvent(AsyncSocket *sock, int e) {
        std::vector<AsyncSocket*>::iterator i = mDataSockets.begin();
        for (; i != mDataSockets.end(); ++i) {
            if (*i == sock) {
                mDataSockets.erase(i);
            }
        }
        sock->Close();
        delete sock;
    }

    talk_base::scoped_ptr<AsyncSocket> mListenSocket;

    std::vector<AsyncSocket*> mDataSockets;

};


int main(int argc, char** argv) {
    FlagList::SetFlagsFromCommandLine(&argc, argv, true);
    if (FLAG_help) {
      FlagList::Print(NULL, false);
      return 0;
    }

    // Abort if the user specifies a port that is outside the allowed
    // range [1, 65535].
    if ((FLAG_port < 1) || (FLAG_port > 65535)) {
      printf("Error: %i is not a valid port.\n", FLAG_port);
      return -1;
    }

    const SocketAddress kServerAddr("127.0.0.1", FLAG_port);
    EchoTcpServer es;
    es.init(Thread::Current(), kServerAddr);

    Thread::Current()->ProcessMessages(kForever);

    return 0;
}
