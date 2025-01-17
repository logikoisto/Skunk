#include "address.h"

#include <arpa/inet.h>

#include <sstream>

#include "string.h"
namespace zoo {
namespace skunk {
IpAddress::IpAddress(std::string ip, in_port_t port) {
  bzero(&addr_, sizeof addr_);
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  int s;
  if ((s == ::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0)) {
    if (s == 0) {
      // log
    } else {
      // log
    }
  }
}
IpAddress::IpAddress(in_port_t port) {
  bzero(&addr_, sizeof addr_);
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  addr_.sin_addr.s_addr = ::htonl(INADDR_ANY);
}

IpAddress::IpAddress(const struct sockaddr_in& addr) : addr_(addr) {}

std::string IpAddress::toString() const {
  std::stringstream ss;
  char buf[20];
  const char* ip = ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  if (ip) {
    ss << ip;
  } else {
    ss << "invalid ip";
  }
  ss << ":" << ntohs(addr_.sin_port);
  return ss.str();
}
const struct sockaddr* IpAddress::GetSockAddr() const {
  const void* void_ptr = static_cast<const void*>(&addr_);
  return static_cast<const struct sockaddr*>(void_ptr);
};
struct sockaddr* IpAddress::GetSockAddr() {
  void* void_ptr = static_cast<void*>(&addr_);
  return static_cast<struct sockaddr*>(void_ptr);
}

}  // namespace skunk
}  // namespace zoo