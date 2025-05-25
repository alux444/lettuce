#include "../include/LettuceServer.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

static LettuceServer *globalServer = nullptr;

LettuceServer::LettuceServer(int port) : port(port), serverSocket(-1), isRunning(true)
{
  globalServer = this;
}

void LettuceServer::shutdown()
{
  isRunning = false;
  if (serverSocket != -1)
  {
    close(serverSocket);
  }
  std::cout << "Server shutdown." << std::endl;
}

void LettuceServer::run()
{
  // AF_INET = IPv4 addressing, SOCK_STREAM = TCP socket, 0 = default protocol
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0)
  {
    std::cerr << "Failed to create socket." << std::endl;
    return;
  }

  int option = 1;
  setsockopt(serverSocket,
             SOL_SOCKET,    // general socket option (level of option)
             SO_REUSEADDR,  // allow reuse of address
             &option,       // pointer to option value, 1 = enabled
             sizeof(option) // size of option value);
  );

  sockaddr_in serverAddress{};
  serverAddress.sin_family = AF_INET;         // IPv4
  serverAddress.sin_port = htons(port);       // host to network byte order
  serverAddress.sin_addr.s_addr = INADDR_ANY; // bind to all interfaces
                                              // listening on all IP addresses of the machine for incoming connections

  if (bind(serverSocket,
           (struct sockaddr *)&serverAddress,
            sizeof(serverAddress)) < 0)
  {
    std::cerr << "Failed to bind socket." << std::endl;
    return;
  }

  if (listen(serverSocket, 10) < 0)
  {
    std::cerr << "Error listening on socket." << std::endl;
    return;
  }

  std::cout << "Lettuce server listening on port " << port << std::endl;
}