/**
 * @file channel.h
 * @author Jamishon
 * @brief Manage the send and receive data of a socket file discription
 * @version 0.1
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <arpa/inet.h>
#include <string>
#include <queue>
#include <memory>
#include "epoller.h"

class EPoller;

class Channel : public std::enable_shared_from_this<Channel> {
public:
    explicit Channel(int fd);
    virtual ~Channel();

    virtual void Send(void *buf, std::size_t buf_len);
    bool Send();
    virtual int Receive();
    std::string GetIp();
    int GetPort();
    void set_sock_addr(sockaddr_in sock_addr);
    sockaddr_in get_sock_addr();
    int fd();
    int CheckWriteBuffer(std::vector<char> &buf, int append_len);
    int CheckReadBuffer(std::vector<char> &buf);
    void OnError(int epoll_id, int err_no);
    void OnClose();
    void SetEPoller(std::shared_ptr<EPoller> epoller);

    const std::size_t kInitBufSize = 1024;
    const std::size_t kMaxBufSize = kInitBufSize * 2;

protected:
    const int fd_;
    sockaddr_in sock_addr_;
    std::vector<char> send_buf_;
    std::vector<char> recv_buf_;
    std::shared_ptr<EPoller> epoller_;
};

#endif