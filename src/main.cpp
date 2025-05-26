#include <iostream>
#include "../include/LettuceServer.h"
#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
  int port = 6379;

  // if custom port is defined
  if (argc >= 2)
  {
    port = std::stoi(argv[1]);
  }

  LettuceServer server(port);

  // every 5 mins save database
  std::thread persistenceThread([](){
    while (true)
    {
      std::this_thread::sleep_for(std::chrono::minutes(5));
      // TODO: dump the database
      std::cout << "Database saved." << std::endl; 
    }
  });
  persistenceThread.detach();

  server.run();

  return 0;
}