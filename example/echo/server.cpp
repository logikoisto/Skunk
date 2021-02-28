#include <stdint.h>
#include <sys/types.h>

#include <string>

#include "skunk/address.h"
#include "skunk/tcp_server.h"

int main() {
  zoo::skunk::IpAddress *addr = new zoo::skunk::IpAddress("127.0.0.1", 8080);
  zoo::skunk::TcpServer *server = new zoo::skunk::TcpServer(*addr, 10);
  server->Run();
}