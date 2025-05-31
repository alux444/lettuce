#include <iostream>
#include "../include/LettuceServer.h"
#include "../include/LettuceDatabase.h"
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

  std::string databaseFilename = "dump.ldb";

  if (LettuceDatabase::getInstance().load(databaseFilename))
  {
    std::cout << "Database loaded from dump.ldb\n";
  }
  else
  {
    std::cout << "No dump.ldb file found\n";
  }

  LettuceServer server(port);

  // every 5 mins save database
  std::thread persistenceThread([](){
    while (true)
    {
      std::this_thread::sleep_for(std::chrono::minutes(5));
      if (!LettuceDatabase::getInstance().dump("dump.ldb"))
      {
        std::cerr << "-ERR: Failed to dump database." << std::endl;
        continue;
      }
      std::cout << "Database dumped to dump.ldb" << std::endl; 
    }
  });
  persistenceThread.detach();

  server.run();

  return 0;
}