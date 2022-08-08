/**
 * @file channel.cc
 * @author Jamishon
 * @brief Basic communication unit
 * @version 0.1
 * @date 2021-12-06
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "channel.h"

#include <errno.h>
#include <string.h>
#include <sys/epoll.h>

#include <string>

#include "log/LogWrapper.h"
#include "socket.h"

Channel::Channel(int fd) : fd_(fd) { LOG_INFO("Channel::Channel fd:" << fd); }

Channel::~Channel() {
  send_buf_.clear();
  recv_buf_.clear();
  welink::socket::Close(fd_);

  LOG_INFO("Channel::~Channel fd:" << fd_);
}

void Channel::Send(void *buf, std::size_t len) {
  if (buf != nullptr && len > 0) {
    if (CheckWriteBuffer(send_buf_, len) < 0) return;

    int size = send_buf_.size();
    send_buf_.reserve(size + len);
    const char *src = static_cast<const char *>(buf);
    std::copy(src, src + len, std::back_inserter(send_buf_));

    Send();
  }
}

bool Channel::Send() {
  bool on_send = true;
  while (on_send) {
    int send_len = send_buf_.size();
    if (send_len <= 0) {
      epoll_event event;
      event.data.fd = fd_;
      event.events = EPOLLIN;
      epoller_->EPollCtrl(shared_from_this(), EPOLL_CTL_MOD, &event);

      break;
    }

    int ret = welink::socket::Write(fd_, send_buf_.data(), send_len);
    if (ret >= 0) {
      std::stringstream ss;
      for (auto it = send_buf_.begin(); it != send_buf_.begin() + ret; it++) {
        ss << *it;
      }
      LOG_INFO("Channel::send write ret:" << ret << " send data:" << ss.str());
      send_buf_.erase(send_buf_.begin(), send_buf_.begin() + ret);

    } else if (ret < 0) {
      int err_no = errno;
      if (err_no == EWOULDBLOCK) {
        /* LT notify*/
        epoll_event event;
        event.data.fd = fd_;
        event.events = EPOLLIN | EPOLLOUT;
        epoller_->EPollCtrl(shared_from_this(), EPOLL_CTL_MOD, &event);
      } else {
        return false;
      }

      on_send = false;
    }
  }

  return true;
}

int Channel::fd() { return fd_; }

int Channel::Receive() {
  bool on_read = true;
  int err_no = 0;

  while (on_read) {
    if (CheckReadBuffer(recv_buf_) < 0) break;
    /**TODO OPTIMIZE receive data**/
    char buffer[kInitBufSize];
    ::memset(buffer, 0, kInitBufSize);

    int ret = welink::socket::Read(fd_, buffer, kInitBufSize);
    if (ret > 0) {
      auto size = recv_buf_.size();
      recv_buf_.reserve(size + ret);
      std::copy(buffer, buffer + ret, std::back_inserter(recv_buf_));

      /**TODO UNDO notify receive data**/
      std::stringstream ss;
      for (char c : recv_buf_) {
        ss << c;
      }

      std::string temp(ss.str());
      LOG_INFO("Read data fd:" << fd_ << " ret:" << ret << " data:" << temp);

      int len = temp.length();
      LOG_INFO(temp << " " << len);
      Send(static_cast<void *>(const_cast<char *>(temp.c_str())), len);

      recv_buf_.clear();

    } else if (ret == 0) {
      // client disconnect, receive EPOLLIN and ret is 0
      on_read = false;
      OnClose();
    } else {
      // ret == -1
      err_no = errno;
      on_read = false;
      if (err_no == EWOULDBLOCK || err_no == EINTR) {
        // LT read
      } else {
        OnError(epoller_->epfd(), err_no);
      }

     
    }

    LOG_INFO("Channel::Receive read(2) ret:" << ret << " errno:" << err_no );
  }

  return 0;
}

int Channel::CheckWriteBuffer(std::vector<char> &buf, int append_len) {
  std::size_t will_len = buf.size() + append_len;
  if (will_len > kMaxBufSize) {
    return -1;
  } else {
    return 0;
  }
}

int Channel::CheckReadBuffer(std::vector<char> &buf) {
  auto size = buf.size();
  if (size >= kMaxBufSize)
    return -1;
  else
    return 0;
}

void Channel::OnError(int epoll_id, int err_no) {
  LOG_ERROR("Channel::OnError epoll fd:" << epoll_id << " errno"
                                            << err_no << " fd:" << fd_);
  OnClose();                                        
}

void Channel::OnClose() {
  int err_no = errno;
  LOG_ERROR("Channel::OnClose epoll fd:" << epoller_->epfd() << " errno"
                                            << err_no << " fd:" << fd_);
  epoll_event event;
  event.data.fd = fd_;
  event.events = 0;
  epoller_->EPollCtrl(shared_from_this(), EPOLL_CTL_DEL, &event);
  welink::socket::Close(fd_);  
}

void Channel::set_sock_addr(sockaddr_in addr) { sock_addr_ = addr; }

std::string Channel::GetIp() { 
  char buf[64] = {0};
  socklen_t len = sizeof(sock_addr_);
  ::inet_ntop(AF_INET, &(sock_addr_.sin_addr.s_addr), buf, len);

  return std::string(buf);
}

int Channel::GetPort() { 

  return ::ntohs(sock_addr_.sin_port); 
  
}

void Channel::SetEPoller(std::shared_ptr<EPoller> epoller) {
  epoller_ = epoller;

  epoll_event event;
  event.data.fd = fd_;
  event.events = EPOLLIN;
  std::shared_ptr<Channel> shared_this = shared_from_this();
  epoller_->EPollCtrl(shared_this, EPOLL_CTL_ADD, &event);
}
