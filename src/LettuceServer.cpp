#include "../include/LettuceServer.h"
#include "../include/LettuceCommandHandler.h"
#include "../include/LettuceDatabase.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <signal.h>

#define DEBUG 0

static LettuceServer *globalServer = nullptr;

void signalHandler(int signum)
{
  if (globalServer)
  {
    std::cout << "Caught signal: " << signum << ", shutting down..." << std::endl;
    globalServer->shutdown();
  }
  exit(signum);
}

void LettuceServer::setupSignalHandler()
{
  signal(SIGINT, signalHandler);
}

LettuceServer::LettuceServer(int port) : port(port), serverSocket(-1), isRunning(true)
{
  globalServer = this;
  setupSignalHandler();
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

  std::vector<std::thread> threads;
  LettuceCommandHandler commandHandler;
  while (isRunning)
  {
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    std::cout << "Client connected." << std::endl;

    if (clientSocket < 0)
    {
      if (!isRunning)
      {
        std::cerr << "-ERR Accepting client connection" << std::endl;
        break;
      }
    }

    // start new thread and add to thread vector
    // take clientsocket by value so each thread owns its copy of socket
    // commandhandler as reference, because we want to share it between threads
    threads.emplace_back([clientSocket, &commandHandler]()
                         {
        char buffer[1024];
        while (true)
        {
          memset(buffer, 0, sizeof(buffer)); // clear the buffer

          std::cout << "Waiting for command..." << std::endl;
          int receivedBytes = recv(clientSocket, buffer, sizeof(buffer)-1, 0); // read up to 1023 bytes from the client
          std::cout << "Received " << receivedBytes << " bytes." << std::endl;

          if (receivedBytes <= 0)
          {
            std::cerr << "-ERR Failed to receive data or client disconnected." << std::endl;
            close(clientSocket);
            return;
          }

          std::string request(buffer, receivedBytes); // construct string from received bytes
          std::string response = commandHandler.handleCommand(request);
          send(clientSocket, response.c_str(), response.size(), 0); // send response back to client
          std::cout << "Sent response: " << response << std::endl;
        }
        close(clientSocket); });

    for (auto &thread : threads)
    {
      if (thread.joinable())
        thread.join();
    }

    // if (LettuceDatabase::getInstance().dump("dump.ldb"))
    // {
    //   std::cout << "Database dumped to dump.ldb" << std::endl;
    // }
    // else
    // {
    //   std::cout << "-ERR failed to dump database" << std::endl;
    // }
  }
}