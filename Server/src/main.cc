
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "ConfigFile.h"
#include "Singleton.h"
#include "log/LogWrapper.h"

#define SERVER_NAME "welink"
#define EPOLL_MAX_EVENTS 1024

int main(int argc, char *argv[]) {
  ConfigFile file("../etc/chatserver.conf");
  //   std::string value = file.GetConfig("dbserver");
  //   std::cout << value << std::endl;

  bool log_ret =
      Singleton<LogWrapper>::Instance().Init("../etc/log.conf", SERVER_NAME);
  LOG_INFO("WeLink log function loaded");

  {
    // socket fd create
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
      LOG_ERROR("socket() error!");
      return -1;
    }

    // reuse ip and port
    int opt = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof opt);
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof opt);

    // set socket nonblock
    int flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);

    // init ip and port
    unsigned short port = std::stoi(file.GetConfig("listenport"));
    std::string ip = file.GetConfig("listenip");

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // bind
    int ret = bind(fd, (sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
      LOG_ERROR("bind error, ip:" << ip << " port:" << port);
      close(fd);
      return -1;
    }

    // listen
    ret = listen(fd, SOMAXCONN);
    if (ret == -1) {
      LOG_ERROR("listen error, ip:" << ip << " port:" << port);
      close(fd);
      return -1;
    }

    // epoll fd create
    int epfd = epoll_create(1);
    if (epfd == -1) {
      LOG_ERROR("epoll_create() error");
      close(fd);
      return -1;
    }

    // add epoll event
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;  // | EPOLLOUT;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1) {
      LOG_ERROR("epoll_ctl() error epid:" << epfd << " fd:" << fd);
      close(fd);
      return -1;
    }

    std::vector<char> vc_sendbuf;
    while (true) {
      // epoll event
      epoll_event events[EPOLL_MAX_EVENTS];
      int num = epoll_wait(epfd, events, EPOLL_MAX_EVENTS, 1000);
      if (num == 0) {
        continue;
      } else if (num < 0) {
        if (errno == EINTR || errno == EAGAIN)
          continue;
        else
          break;
      }

      for (int i = 0; i < num; i++) {
        epoll_event ev = events[i];

        if (ev.events & EPOLLIN) {
          if (ev.data.fd == fd) {
            sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(sockaddr_in));
            socklen_t sock_len = sizeof(sockaddr_in);
            int client_fd = accept(fd, (sockaddr *)&client_addr, &sock_len);
            if (client_fd == -1) {
              LOG_ERROR("accept error severfd:" << fd);
              continue;
            }

            int flag = fcntl(client_fd, F_GETFL, 0);
            flag |= O_NONBLOCK;
            fcntl(client_fd, F_SETFL, flag);

            epoll_event client_event;
            client_event.data.fd = client_fd;
            client_event.events = EPOLLIN;  // | EPOLLOUT;
            // client_event.events |= EPOLLET;

            if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_event) ==
                -1) {
              close(client_fd);
              LOG_ERROR("epoll_ctl() add error epfd:" << epfd
                                                      << " client_fd:" << fd);
              continue;
            }
            std::cout << "accept fd:" << client_fd << std::endl;
          } else {
            char buf[5];

            // {  // LT read
            //   ::memset(buf, 0, sizeof buf);
            //   int ret = ::read(ev.data.fd, buf, sizeof buf);
            //   // int ret = ::recv(ev.data.fd, buf, sizeof buf, 0);
            //   if (ret > 0) {
            //     LOG_INFO("read() client data from fd:" << ev.data.fd
            //                                            << " data:" << buf);
            //     std::cout << "read() fd:" << ev.data.fd << " data:" << buf
            //               << std::endl;

            //     {  // send back
            //       for (unsigned int i = 0; i < sizeof(buf); i++) {
            //         if (buf[i] == '\0') break;
            //         vc_sendbuf.push_back(buf[i]);
            //       }
            //       epoll_event temp;
            //       temp.data.fd = ev.data.fd;
            //       temp.events = EPOLLIN | EPOLLOUT;
            //       ::epoll_ctl(epfd, EPOLL_CTL_MOD, ev.data.fd, &temp);
            //     }

            //   } else if (ret == 0) {
            //     // EOF
            //     ::epoll_ctl(epfd, EPOLL_CTL_DEL, fd, 0);
            //     ::close(ev.data.fd);
            //   } else {
            //     // read error
            //     int err_no = errno;
            //     LOG_ERROR("read error client fd:" << ev.data.fd
            //                                       << " errno:" << err_no);
            //     if (err_no != EWOULDBLOCK && err_no != EINTR) {
            //       ::close(ev.data.fd);
            //     }
            //   }
            // }

            {  // ET read
              bool read_end = false;
              while (!read_end) {
                ::memset(buf, 0, sizeof buf);
                int ret = ::read(ev.data.fd, buf, sizeof buf);
                // int ret = ::recv(ev.data.fd, buf, sizeof buf, 0);
                if (ret > 0) {
                  LOG_INFO("read() client data from fd:" << ev.data.fd
                                                         << " data:" << buf);
                  std::cout << "read() fd:" << ev.data.fd << " data:" << buf
                            << std::endl;

                  {  // send back
                    for (unsigned int i = 0; i < sizeof(buf); i++) {
                      if (buf[i] == '\0') break;
                      vc_sendbuf.push_back(buf[i]);
                    }
                    epoll_event temp;
                    temp.data.fd = ev.data.fd;
                    temp.events = EPOLLIN | EPOLLOUT | EPOLLET;
                    ::epoll_ctl(epfd, EPOLL_CTL_MOD, ev.data.fd, &temp);
                  }

                } else if (ret == 0) {
                  LOG_INFO("read() EOF fd:" << ev.data.fd);
                  // EOF
                  ::epoll_ctl(epfd, EPOLL_CTL_DEL, ev.data.fd, 0);
                  ::close(ev.data.fd);
                  read_end = true;
                } else {
                  // read error, notice the way to store errno, errno will 
                  // be set to zero 
                  int err_no = errno;
                  LOG_ERROR("read error client fd: " << ev.data.fd
                                  << " errno:" << err_no);

                  if (err_no != EWOULDBLOCK && err_no != EINTR) {
                    ::close(ev.data.fd);
                  }
                  read_end = true;
                }
              }
            }

            // set ET mode EPOLLOUT for test
            // epoll_event e_event;
            // e_event.data.fd = ev.data.fd;
            // e_event.events = EPOLLOUT | EPOLLIN | EPOLLET;
            // epoll_ctl(epfd, EPOLL_CTL_MOD, ev.data.fd, &e_event);
          }

        } else if (ev.events & EPOLLOUT) {
          LOG_INFO("epollout fd:" << ev.data.fd);

          if (ev.data.fd == fd) {
          } else {
            // {  // LT send

            //   LOG_INFO("EPOLLOUT, sendbuf len:" << vc_sendbuf.size());

            //   if (vc_sendbuf.size() <= 0) {
            //     epoll_event temp;
            //     temp.data.fd = ev.data.fd;
            //     temp.events = EPOLLIN;
            //     ::epoll_ctl(epfd, EPOLL_CTL_MOD, ev.data.fd, &temp);

            //     continue;
            //   }

            //   char buf[32] = {0};
            //   int len = 0;
            //   for (unsigned int i = 0; i < vc_sendbuf.size() && i < 32; i++)
            //   {
            //     buf[i] = vc_sendbuf[i];
            //     len++;
            //   }
            //   int ret = ::write(ev.data.fd, buf, len);

            //   if (ret > 0 && ret <= len) {
            //     vc_sendbuf.erase(vc_sendbuf.begin(), vc_sendbuf.begin() +
            //     ret);

            //   } else if (ret == 0) {
            //   } else {
            //   }

            //    LOG_INFO("write() ret:" << ret << " sendbuf len:" <<
            //    vc_sendbuf.size());
            // }

            {  // ET send
              if (vc_sendbuf.size() <= 0) {
                continue;
              } else {
                epoll_event temp;
                temp.data.fd = ev.data.fd;
                temp.events = EPOLLIN | EPOLLOUT | EPOLLET;
                ::epoll_ctl(epfd, EPOLL_CTL_MOD, ev.data.fd, &temp);
              }

              char buf[32] = {0};
              int len = 0;
              for (unsigned int i = 0; i < vc_sendbuf.size() && i < 32; i++) {
                buf[i] = vc_sendbuf[i];
                len++;
              }
              int ret = ::write(ev.data.fd, buf, len);

              if (ret > 0 && ret <= len) {
                vc_sendbuf.erase(vc_sendbuf.begin(), vc_sendbuf.begin() + ret);

              } else if (ret == 0) {
              } else {
              }

              LOG_INFO("write() ret:" << ret
                                      << " sendbuf len:" << vc_sendbuf.size());
            }

            // static int mark = 0;
            // std::stringstream ss;
            // ss << "hello world " << mark++;
            // int ret = ::write(ev.data.fd, ss.str().data(),
            // ss.str().size()); if(ret >= 0) {
            //     LOG_INFO("write to client data fd:" << ev.data.fd << "
            //     data:" << ss.str());
            // } else if ( ret < 0) {
            //     LOG_ERROR("write error client fd:" << ev.data.fd);
            // }
          }

        } else if (ev.events & EPOLLERR) {
          LOG_ERROR("receive error event fd:" << ev.data.fd
                                              << " errno:" << errno);
        }
      }
    }

    close(fd);
  }
}
