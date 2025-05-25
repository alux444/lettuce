#include <iostream>
#include "../include/LettuceServer.h"

int main(int argc, char *argv[])
{
  int port = 6379;

  // if custom port is defined
  if (argc >= 2)
  {
    port = std::stoi(argv[1]);
  }

  LettuceServer server(port);
  server.run();

  return 0;
}