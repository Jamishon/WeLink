/**
 * @file tcpserver.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-12-02
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "tcpserver.h"

#include "socket.h"
#include "channel_listen.h"

TcpServer::TcpServer(std::string ip, int port)
    : channel_listen_(new ChannelListen(welink::socket::CreateNonblockSocket(),
                                        ip, port)) , epoller_(new EPoller()) 
{


}

TcpServer::~TcpServer() {}

void TcpServer::Start() {
    channel_listen_->SetEPoller(epoller_);
    channel_listen_->SetConnectionCallback(con_callback_);
    epoller_->EPollWait();
}

void TcpServer::SetConnectionCallback(const ConnectionCallback &callback) {
    con_callback_ = callback;
}

