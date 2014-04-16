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

#include "XmppSession.h"
#include "xmpptask.h"
#include "talk/base/logging.h"
#include "talk/base/sigslot.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/stringutils.h"

#include "talk/xmpp/xmppauth.h"
#include "talk/xmpp/constants.h"
#include "talk/xmpp/xmppsocket.h"
#include "talk/xmpp/saslplainmechanism.h"
#include "talk/xmpp/prexmppauth.h"
#include "talk/xmpp/plainsaslhandler.h"

#include <utils/Log.h>

namespace buzz {

class XmppSession::Private :
            public sigslot::has_slots<>,
            public XmppSessionHandler,
            public XmppOutputHandler {
  public:

    explicit Private(XmppSession* client, XmppSocket *socket)
            : client_(client)
            , socket_(socket)
            , engine_(NULL)
            , proxy_port_(0)
            , pre_engine_error_(XmppEngine::ERROR_NONE)
            , pre_engine_subcode_(0)
            , signal_closed_(false)
            , allow_plain_(false) {}

    virtual ~Private() {
        // We need to disconnect from socket_ before engine_ is destructed (by
        // the auto-generated destructor code).
        ResetSocket();
    }

    // the owner
    XmppSession* const client_;

    // the two main objects
    talk_base::scoped_ptr<XmppSocket> socket_;
    talk_base::scoped_ptr<XmppEngine> engine_;
    talk_base::scoped_ptr<PreXmppAuth> pre_auth_;
    talk_base::CryptString pass_;
    std::string auth_mechanism_;
    std::string auth_token_;
    talk_base::SocketAddress server_;
    std::string proxy_host_;
    int proxy_port_;
    XmppEngine::Error pre_engine_error_;
    int pre_engine_subcode_;
    CaptchaChallenge captcha_challenge_;
    bool signal_closed_;
    bool allow_plain_;

    void ResetSocket() {
        if (socket_) {
            socket_->SignalConnected.disconnect(this);
            socket_->SignalRead.disconnect(this);
            socket_->SignalClosed.disconnect(this);
            socket_.reset(NULL);
        }
    }

    // implementations of interfaces
    void OnStateChange(int state);
    void WriteOutput(const char* bytes, size_t len);
    void StartTls(const std::string& domainname);
    void CloseConnection();

    // slots for socket signals
    void OnSocketConnected();
    void OnSocketRead();
    void OnSocketClosed();
};

static bool IsTestServer(const std::string& server_name,
                         const std::string& test_server_domain) {
    return (!test_server_domain.empty() &&
            talk_base::ends_with(server_name.c_str(),
                                 test_server_domain.c_str()));
}

XmppEngine::State XmppSession::GetState() const {
    if (!d_->engine_)
        return XmppEngine::STATE_NONE;
    return d_->engine_->GetState();
}

XmppEngine::Error XmppSession::GetError(int* subcode) {
    if (subcode) {
        *subcode = 0;
    }
    if (!d_->engine_)
        return XmppEngine::ERROR_NONE;
    if (d_->pre_engine_error_ != XmppEngine::ERROR_NONE) {
        if (subcode) {
            *subcode = d_->pre_engine_subcode_;
        }
        return d_->pre_engine_error_;
    }
    return d_->engine_->GetError(subcode);
}

const XmlElement* XmppSession::GetStreamError() {
    if (!d_->engine_) {
        return NULL;
    }
    return d_->engine_->GetStreamError();
}

CaptchaChallenge XmppSession::GetCaptchaChallenge() {
    if (!d_->engine_)
        return CaptchaChallenge();
    return d_->captcha_challenge_;
}

std::string XmppSession::GetAuthMechanism() {
    if (!d_->engine_)
        return "";
    return d_->auth_mechanism_;
}

std::string XmppSession::GetAuthToken() {
    if (!d_->engine_)
        return "";
    return d_->auth_token_;
}

int XmppSession::ProcessStart() {
    // Should not happen, but was observed in crash reports
    if (!d_->socket_) {
        LOG(LS_ERROR) << "socket_ already reset";
        return STATE_DONE;
    }

    if (d_->pre_auth_) {
        d_->pre_auth_->SignalAuthDone.connect(this, &XmppSession::OnAuthDone);
        d_->pre_auth_->StartPreXmppAuth(
            d_->engine_->GetUser(), d_->server_, d_->pass_,
            d_->auth_mechanism_, d_->auth_token_);
        d_->pass_.Clear(); // done with this;
        return STATE_PRE_XMPP_LOGIN;
    }
    else {
        d_->engine_->SetSaslHandler(new PlainSaslHandler(
            d_->engine_->GetUser(), d_->pass_, d_->allow_plain_));
        d_->pass_.Clear(); // done with this;
        return STATE_START_XMPP_LOGIN;
    }
}

void XmppSession::OnAuthDone() {
    Wake();
}

int XmppSession::ProcessTokenLogin() {
    // Should not happen, but was observed in crash reports
    if (!d_->socket_) {
        LOG(LS_ERROR) << "socket_ already reset";
        return STATE_DONE;
    }

    // Don't know how this could happen, but crash reports show it as NULL
    if (!d_->pre_auth_) {
        d_->pre_engine_error_ = XmppEngine::ERROR_AUTH;
        EnsureClosed();
        return STATE_ERROR;
    }

    // Wait until pre authentication is done is done
    if (!d_->pre_auth_->IsAuthDone())
        return STATE_BLOCKED;

    if (!d_->pre_auth_->IsAuthorized()) {
        // maybe split out a case when gaia is down?
        if (d_->pre_auth_->HadError()) {
            d_->pre_engine_error_ = XmppEngine::ERROR_AUTH;
            d_->pre_engine_subcode_ = d_->pre_auth_->GetError();
        }
        else {
            d_->pre_engine_error_ = XmppEngine::ERROR_UNAUTHORIZED;
            d_->pre_engine_subcode_ = 0;
            d_->captcha_challenge_ = d_->pre_auth_->GetCaptchaChallenge();
        }
        d_->pre_auth_.reset(NULL); // done with this
        EnsureClosed();
        return STATE_ERROR;
    }

    // Save auth token as a result

    d_->auth_mechanism_ = d_->pre_auth_->GetAuthMechanism();
    d_->auth_token_ = d_->pre_auth_->GetAuthToken();

    // transfer ownership of pre_auth_ to engine
    d_->engine_->SetSaslHandler(d_->pre_auth_.release());
    return STATE_START_XMPP_LOGIN;
}

int XmppSession::ProcessStartXmppLogin() {
    // Should not happen, but was observed in crash reports
    if (!d_->socket_) {
        LOG(LS_ERROR) << "socket_ already reset";
        return STATE_DONE;
    }

    // // Done with pre-connect tasks - connect!
    // if (!d_->socket_->Connect(d_->server_)) {
    //     EnsureClosed();
    //     return STATE_ERROR;
    // }

    return STATE_RESPONSE;
}

int XmppSession::ProcessResponse() {
    // Hang around while we are connected.
    if (!delivering_signal_ &&
        (!d_->engine_ || d_->engine_->GetState() == XmppEngine::STATE_CLOSED))
        return STATE_DONE;
    return STATE_BLOCKED;
}

XmppReturnStatus XmppSession::Disconnect() {
    if (!d_->socket_)
        return XMPP_RETURN_BADSTATE;
    Abort();
    d_->engine_->Disconnect();
    d_->ResetSocket();
    return XMPP_RETURN_OK;
}

int XmppSession::OnSocketAccepted(talk_base::AsyncSocket *socket) {
    XmppSocket *xsocket = XmppSocket::CreateAcceptedSocket(TLS_ENABLED, socket);

    // d_.reset(new Private(this, XmppSocket::CreateAcceptedSocket(TLS_ENABLED, socket)));
    d_.reset(new Private(this, xsocket));
    d_->socket_->SignalRead.connect(d_.get(), &Private::OnSocketRead);
    d_->socket_->SignalClosed.connect(d_.get(), &Private::OnSocketClosed);
    d_->socket_->SignalConnected.connect(d_.get(), &Private::OnSocketConnected);

    d_->engine_.reset(XmppEngine::Create(false));
    d_->engine_->SetSessionHandler(d_.get());
    d_->engine_->SetOutputHandler(d_.get());

    d_->engine_->SetServerDomain(mSettings.host());
    d_->engine_->SetUser(buzz::Jid(mSettings.user(), mSettings.host(), STR_EMPTY));

    if (!mSettings.resource().empty()) {
        d_->engine_->SetRequestedResource(mSettings.resource());
    }
    d_->engine_->SetTls(mSettings.use_tls());
  
    // The talk.google.com server returns a certificate with common-name:
    //   CN="gmail.com" for @gmail.com accounts,
    //   CN="googlemail.com" for @googlemail.com accounts,
    //   CN="talk.google.com" for other accounts (such as @example.com),
    // so we tweak the tls server setting for those other accounts to match the
    // returned certificate CN of "talk.google.com".
    // For other servers, we leave the strings empty, which causes the jid's
    // domain to be used.  We do the same for gmail.com and googlemail.com as the
    // returned CN matches the account domain in those cases.
    std::string server_name = mSettings.server().HostAsURIString();
    if (server_name == buzz::STR_TALK_GOOGLE_COM ||
        server_name == buzz::STR_TALKX_L_GOOGLE_COM ||
        server_name == buzz::STR_XMPP_GOOGLE_COM ||
        server_name == buzz::STR_XMPPX_L_GOOGLE_COM ||
        IsTestServer(server_name, mSettings.test_server_domain())) {
        if (mSettings.host() != STR_GMAIL_COM &&
            mSettings.host() != STR_GOOGLEMAIL_COM) {
            d_->engine_->SetTlsServer("", STR_TALK_GOOGLE_COM);
        }
    }

    // Set language
    d_->engine_->SetLanguage("*");

    d_->engine_->SetUser(buzz::Jid(mSettings.user(), mSettings.host(), STR_EMPTY));

    d_->pass_ = mSettings.pass();
    d_->auth_mechanism_ = mSettings.auth_mechanism();
    d_->auth_token_ = mSettings.auth_token();
    d_->server_ = mSettings.server();
    d_->proxy_host_ = mSettings.proxy_host();
    d_->proxy_port_ = mSettings.proxy_port();
    d_->allow_plain_ = mSettings.allow_plain();
    d_->pre_auth_.reset(new XmppAuth());

    valid_ = true;

    // d_->engine_->Connect();
    xsocket->OnSocketAccepted(socket);
    xsocket->SetPrivateKeyFile(mSettings.certificate_file());
    xsocket->SetCertificateFile(mSettings.certificate_file());
    socket->SignalConnectEvent(socket);


    return 0;
}

XmppSession::XmppSession(TaskParent *parent,
                         const XmppClientSettings &settings)
        : XmppTaskParentInterface(parent)
        , delivering_signal_(false)
        , valid_(false)
        , mSettings(settings) {
}

XmppSession::~XmppSession() {
    valid_ = false;
}

const Jid& XmppSession::jid() const {
    return d_->engine_->FullJid();
}

const Jid& XmppSession::GetPeerUser() const {
    return d_->engine_->GetPeerUser();
}

void XmppSession::SetPeerUser(const Jid &peer) {
    d_->engine_->SetPeerUser(peer);
}

std::string XmppSession::NextId() {
    return d_->engine_->NextId();
}

XmppReturnStatus XmppSession::SendStanza(const XmlElement* stanza) {
    return d_->engine_->SendStanza(stanza);
}

XmppReturnStatus XmppSession::SendStanzaError(
    const XmlElement* old_stanza, XmppStanzaError xse,
    const std::string& message) {
    return d_->engine_->SendStanzaError(old_stanza, xse, message);
}

XmppReturnStatus XmppSession::SendRaw(const std::string& text) {
    return d_->engine_->SendRaw(text);
}

XmppEngine* XmppSession::engine() {
    return d_->engine_.get();
}

void XmppSession::Private::OnSocketConnected() {
    engine_->Connect();
}

void XmppSession::Private::OnSocketRead() {
    char bytes[4096];
    size_t bytes_read;
    for (;;) {
        // Should not happen, but was observed in crash reports
        if (!socket_) {
            LOG(LS_ERROR) << "socket_ already reset";
            return;
        }

        if (!socket_->Read(bytes, sizeof(bytes), &bytes_read)) {
            // TODO: deal with error information
            return;
        }

        if (bytes_read == 0)
            return;

        //#ifdef _DEBUG
        client_->SignalLogInput(bytes, bytes_read);
        //#endif
        hexdump_info(bytes, bytes_read, "OnSocketRead:%d", bytes_read);
        engine_->HandleInput(bytes, bytes_read);
    }
}

void XmppSession::Private::OnSocketClosed() {
    int code = socket_->GetError();
    engine_->ConnectionClosed(code);
}

void XmppSession::Private::OnStateChange(int state) {
    if (state == XmppEngine::STATE_CLOSED) {
        client_->EnsureClosed();
    }
    else {
        client_->SignalStateChange(client_, (XmppEngine::State)state);
    }
    client_->Wake();
}

void XmppSession::Private::WriteOutput(const char* bytes, size_t len) {
    //#ifdef _DEBUG
    client_->SignalLogOutput(bytes, len);
    //#endif

    socket_->Write(bytes, len);
    // TODO: deal with error information
}

void XmppSession::Private::StartTls(const std::string& domain) {
#if defined(FEATURE_ENABLE_SSL)
    socket_->StartTls(domain);
#endif
}

void XmppSession::Private::CloseConnection() {
    socket_->Close();
}

void XmppSession::AddXmppTask(XmppTask* task, XmppEngine::HandlerLevel level) {
    d_->engine_->AddStanzaHandler(task, level);
}

void XmppSession::RemoveXmppTask(XmppTask* task) {
    d_->engine_->RemoveStanzaHandler(task);
}

void XmppSession::EnsureClosed() {
    if (!d_->signal_closed_) {
        d_->signal_closed_ = true;
        delivering_signal_ = true;
        SignalStateChange(this, XmppEngine::STATE_CLOSED);
        delivering_signal_ = false;
    }
}

}  // namespace buzz
