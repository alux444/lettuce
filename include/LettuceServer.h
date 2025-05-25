#ifndef LETTUCE_SERVER_H
#define LETTUCE_SERVER_H

#include <string>
#include <atomic>

class LettuceServer {
  public:
    LettuceServer(int port);
    void run();
    void shutdown();

  private:
    int port;
    int serverSocket;
    std::atomic<bool> isRunning;
};

#endif