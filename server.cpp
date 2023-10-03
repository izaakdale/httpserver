#include "server.hpp"
#include <sstream>
#include <iostream>
#include <map>

namespace http
{
  TcpServer::TcpServer(std::string ip_addr, int port) : m_ipAddr(ip_addr),
                                                        m_port(port),
                                                        m_incomingMessage(),
                                                        m_sockAddr(),
                                                        m_sockAddrLen(sizeof(m_sockAddr))
  {
    startServer();
    startListen();
    for (;;)
    {
      if (acceptConnection() < 0)
      {
        exit(1);
      }
    }
  }

  TcpServer::~TcpServer()
  {
    stopServer();
  }

  void TcpServer::startServer()
  {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
      std::cout << "socket error" << std::endl;
      return;
    }

    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(m_port);
    m_sockAddr.sin_addr.s_addr = inet_addr(m_ipAddr.c_str());
    if (bind(m_socket, (sockaddr *)&m_sockAddr, m_sockAddrLen) < 0)
    {
      std::cout << "bind error" << std::endl;
      return;
    }
  }

  void TcpServer::startListen()
  {
    if (listen(m_socket, 20) < 0)
    {
      std::cout << "listen error" << std::endl;
      return;
    }

    std::cout << "\n*** Listening on " << inet_ntoa(m_sockAddr.sin_addr) << ":" << ntohs(m_sockAddr.sin_port) << " ***" << std::endl;
  }

  const int BUFF_SIZE = 1024;
  int TcpServer::acceptConnection()
  {
    m_clientSocket = accept(m_socket, (sockaddr *)&m_sockAddr, &m_sockAddrLen);
    if (m_clientSocket < 0)
    {
      std::cout << "accept error" << std::endl;
      return -1;
    }

    char buff[BUFF_SIZE];
    long bytesRcv = read(m_clientSocket, buff, BUFF_SIZE);
    if (bytesRcv < 0)
    {
      std::cout << "read error" << std::endl;
      return -1;
    }

    std::string req = buff;
    std::string serverResp = routeRequest(req);

    long bytesSent = write(m_clientSocket, serverResp.c_str(), serverResp.size());
    if (bytesSent < 0)
    {
      std::cout << "write error" << std::endl;
      return -1;
    }
    close(m_clientSocket);
    return 0;
  }

  void TcpServer::stopServer()
  {
    close(m_socket);
    exit(0);
  }

  std::string TcpServer::composeResponse(std::string message)
  {
    std::ostringstream ret;
    ret << "HTTP/1.1 200 OK" << std::endl;
    ret << std::endl;
    ret << message;

    return ret.str();
  }

  std::string TcpServer::routeRequest(std::string req)
  {
    // get first line of request
    std::string line = req.substr(0, req.find("\n"));

    // split by " " to separate method and path
    std::string delimiter = " ";
    std::string method = line.substr(0, line.find(delimiter));
    line.erase(0, line.find(delimiter) + delimiter.length());
    std::string path = line.substr(0, line.find(delimiter));

    if (path == "/")
    {
      return homeHandler(method, req);
    }
    else
    {
      std::ostringstream ret;
      ret << "HTTP/1.1 404 Not Found" << std::endl;
      ret << std::endl;
      ret << "404 Not Found";
      return ret.str();
    }
  }

  enum Method
  {
    UNDEFINED,
    GET,
    POST,
  };

  static std::map<std::string, Method> s_HttpMethodMap;

  Method convert(std::string methodStr)
  {
    if (methodStr == "GET")
      return GET;
    if (methodStr == "POST")
      return POST;

    return UNDEFINED;
  }

  std::string TcpServer::homeHandler(std::string method, std::string req)
  {
    switch (convert(method))
    {
    case GET:
      return composeResponse("hello from get!");
      break;
    case POST:
      return composeResponse("hello from post!");
      break;
    default:
      std::ostringstream ret;
      ret << "HTTP/1.1 405 Method Not Allowed" << std::endl;
      ret << std::endl;
      ret << "405 Method Not Allowed";
      return ret.str();
      break;
    }
  }

} // namespace http
