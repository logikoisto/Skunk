#include "socket.h"

#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#include "address.h"
namespace zoo {
namespace skunk {
Socket::~Socket() { Close(); }
// 绑定ip 和端口信息
void Socket::Bind(const IpAddress& local) {
  if (::bind(fd_, local.GetSockAddr(), sizeof(struct sockaddr_in)) < 0) {
    // TODO: 打印日志 bind: strerror(errno)
  }
}
/* listen 当前socket对象 会设置其为服务端socket对象 */
void Socket::Listen() {
  if (::listen(fd_, SOMAXCONN) < 0) {
    // TODO: 打印日志 bind: strerror(errno)
  }
}
/* socket阻塞在peer地址 直到返回代表新建连接的文件句柄 */
int32_t Socket::Accept(IpAddress& peer) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof(struct sockaddr));
  int connfd = ::accept(fd_, peer.GetSockAddr(), &addrlen);
  if (connfd < 0) {
    // TODO: 打印日志 bind: strerror(errno)
  }
  return connfd;
}
/* 客户端socket调用此函数完成与远程服务端的握手 */
int32_t Socket::Connect(const IpAddress& server_addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof(struct sockaddr));
  return ::connect(fd_, server_addr.GetSockAddr(), addrlen);
}
/* 返回当前socket的地址 */
int32_t Socket::Fd() const { return fd_; }
/* 对底层socket 对应的tcp协议栈进行设置 */
void Socket::SetTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval,
                         static_cast<socklen_t>(sizeof optval));
  if (ret == -1) {
    // TODO: 打印日志 bind: strerror(errno)
  }
}

void Socket::SetReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval,
                         static_cast<socklen_t>(sizeof optval));
  if (ret == -1) {
    // TODO: 打印日志 bind: strerror(errno)
  }
};
void Socket::SetReusePort(bool on) {
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval,
                         static_cast<socklen_t>(sizeof optval));
  if (ret == -1) {
    // TODO: 打印日志 bind: strerror(errno)
  }
};
void Socket::SetKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval,
                         static_cast<socklen_t>(sizeof optval));
  if (ret == -1) {
    // TODO: 打印日志 bind: strerror(errno)
  }
}

/* 提供对内核socket读写的封装 支持分散/聚集io的读写 来兼容ring buffer */
ssize_t Socket::Read(void* buf, size_t count) {
  ssize_t n = ::read(fd_, buf, count);
  if (n < 0) {
    int err = GetSocketError(fd_);
    // TODO: 打印日志 bind: strerror(errno)
  }
  return n;
}

ssize_t Socket::Readv(const struct iovec* iov, int iovcnt) {
  ssize_t n = ::readv(fd_, iov, iovcnt);
  if (n < 0) {
    int err = GetSocketError(fd_);
    // TODO: 打印日志 bind: strerror(errno)
  }
  return n;
}

ssize_t Socket::Write(const void* buf, size_t count) {
  ssize_t n = ::write(fd_, buf, count);
  if (n < 0) {
    int err = GetSocketError(fd_);
    // TODO: 打印日志 bind: strerror(errno)
  }
  return n;
}

ssize_t Socket::Writev(const struct iovec* iov, int iovcnt) {
  ssize_t n = ::write(fd_, iov, iovcnt);
  if (n < 0) {
    int err = GetSocketError(fd_);
    // TODO: 打印日志 bind: strerror(errno)
  }
  return n;
}

/* shutdown 会立即关闭内核的socket 并唤醒在其上等待的线程 */
void Socket::ShutdownWrite() {
  if (::shutdown(fd_, SHUT_WR) < 0) {
    // TODO: 打印日志 bind: strerror(errno)
  }
}

/* 将当前的socket设置为非阻塞 并且不允许在fork的子进程中被继承
 * fork的子进程会close socket*/
void Socket::SetNonBlockAndCloseOnExec() {
  int flags = ::fcntl(fd_, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(fd_, F_SETFL, flags);
  if (ret == -1) {
    // TODO: 打印日志 bind: strerror(errno)
  }

  flags = ::fcntl(fd_, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(fd_, F_SETFD, flags);
  if (ret == -1) {
    // TODO: 打印日志 bind: strerror(errno)
  }
}

/* close 会减少 内核socket的引用计数 当其为0时才会真正的关闭内核的socket */
void Socket::Close() { ::close(fd_); }

// 静态函数创建一个 socket 返回内核的fd值
Socket::Ptr Socket::CreateTcp() {
  int fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    // TODO: 打印日志 socket: strerror(errno)
  }
  return std::make_shared<Socket>(int32_t(fd));
}
/* 获取fd上的执行错误号 */
int32_t Socket::GetSocketError(const int32_t sockfd) {
  int32_t optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;  // Solaris
  } else {
    errno = optval;
    return optval;  // Berkeley
  }
}
}  // namespace skunk
}  // namespace zoo