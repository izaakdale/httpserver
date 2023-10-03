#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <map>

namespace http
{
  class TcpServer
  {
  private:
    int m_socket;
    int m_clientSocket;
    std::string m_ipAddr;
    int m_port;
    long m_incomingMessage;
    struct sockaddr_in m_sockAddr;
    unsigned int m_sockAddrLen;

    void startServer();
    void startListen();
    int acceptConnection();
    void stopServer();
    std::string composeResponse(std::string message);
    std::string routeRequest(std::string req);

    std::string homeHandler(std::string method, std::string req);

  public:
    TcpServer(std::string ip_addr, int port);
    ~TcpServer();
  };
} // namespace http