//
// Created by logic on 2021/1/12.
//

#ifndef _SKUNK_SOCKET_H_
#define _SKUNK_SOCKET_H_
#include <memory>

#include "address.h"
namespace zoo {
namespace skunk {
/**
socket 是一个封装底层网络套接字的类
*/

class Socket {
 public:
  using Ptr = std::shared_ptr<Socket>;
  explicit Socket(int fd) : fd_(fd){};
  /* 绑定地址 创建一个 socket对象 此时socket对象 默认为客户端socket*/
  void Bind(const IpAddress& local);
  /* listen 当前socket对象 会设置其为服务端socket对象 */
  void Listen();
  /* socket阻塞在peer地址 直到返回代表新建连接的文件句柄 */
  int32_t Accept(IpAddress& peer);
  /* 客户端socket调用此函数完成与远程服务端的握手 */
  int32_t Connect(IpAddress& server_addr);
  /* 返回当前socket的地址 */
  int32_t Fd() const;

  /* 对底层socket 对应的tcp协议栈进行设置 */
  void SetTcpNoDelay(bool on);
  void SetReuseAddr(bool on);
  void SetReusePort(bool on);
  void SetKeepAlive(bool on);

  /* 提供对内核socket读写的封装 支持分散/聚集io的读写 来兼容ring buffer */
  ssize_t Read(void* buf, size_t count);
  ssize_t Readv(const struct iovec* iov, int iovcnt);
  ssize_t Write(const void* buf, size_t count);
  ssize_t Writev(const struct iovec* iov, int iovcnt);

  /* shutdown 会立即关闭内核的socket 并唤醒在其上等待的线程 */
  void ShutdownWrite();
  /* close 会减少 内核socket的引用计数 当其为0时才会真正的关闭内核的socket */
  void Close();

  /* 将当前的socket设置为非阻塞 并且不允许在fork的子进程中被继承
   * fork的子进程会close socket*/
  void SetNonBlockAndCloseOnExec();
  /* 一个静态方法 底层执行 socket系统调用 创建一个tcp的 socket对象*/
  static Socket::Ptr CreateTcp();
  /* 获取fd上的执行错误号 */
  static int32_t GetSocketError(int sockfd);

 private:
  /* 底层socket的文件句柄 */
  int32_t fd_;
};
}  // namespace skunk
}  // namespace zoo
#endif  // _SKUNK_SOCKET_H_
