/**
 * @file connection.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-12-08
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "connection.h"

Connection::Connection(int fd) : channel_(new Channel(fd)) {}

Connection::~Connection() {}

int Connection::Connect() { return 0; }

void Connection::Disconnect() { channel_->OnClose(); }

std::shared_ptr<Channel> Connection::GetChannel() { return channel_; }

void Connection::SendData(void *buf, int buf_len) {
  channel_->Send(buf, buf_len);
}

void Connection::SetReadDataCallback(ReadCallback call_back) {
  read_data_ = call_back;
}

void Connection::SetSendDataCompleteCallback(
    SendDataCompleteCallback callback) {
  send_data_complete_ = callback;
}