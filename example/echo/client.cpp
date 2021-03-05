#include <stdint.h>
#include <sys/types.h>

#include <string>

#include "skunk/address.h"
#include "skunk/callbacks.h"
#include "skunk/connection.h"
#include "skunk/tcp_client.h"

int main() {
  zoo::skunk::IpAddress *addr = new zoo::skunk::IpAddress("127.0.0.1", 8080);
  zoo::skunk::TcpClient *client = new zoo::skunk::TcpClient();
  client->RegisterMessageHandler(
      [](const std::shared_ptr<zoo::skunk::Connection> conn) -> void {});
  auto conn = client->Connect(*addr);
  conn->Send("XXXX", 3);
}