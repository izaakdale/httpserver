#include "server.hpp"

int main(int argc, char const *argv[])
{
  http::TcpServer srv = http::TcpServer("127.0.0.1", 8080);

  return 0;
}
