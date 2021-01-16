#ifndef __SKUNK_ADDRESS_H__
#define __SKUNK_ADDRESS_H__

#include <netinet/in.h>

#include <string>
namespace zoo {
namespace skunk {
/**
 * This is an ip address class
 */
class IpAddress final {
 public:
  /**
   * 构造一个 服务端socket的地址信息 ip:port
   */
  IpAddress(std::string ip, in_port_t port);
  explicit IpAddress(in_port_t port = 0);
  explicit IpAddress(const struct sockaddr_in &addr);

  /**
   * default dtor, copy ctor, assign operation is ok
   */
  std::string ToString() const;
  const struct sockaddr *GetSockAddr() const;
  struct sockaddr *GetSockAddr();

 private:
  /**
   * linux 的地址信息结构
   */
  struct sockaddr_in addr_;
};

}  // namespace skunk
}  // namespace zoo

#endif  // __SKUNK_ADDRESS_H__