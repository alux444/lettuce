#ifndef LETTUCE_COMMAND_HANDLER_H
#define LETTUCE_COMMAND_HANDLER_H

#include <string>

class LettuceCommandHandler
{
public:
  LettuceCommandHandler();
  std::string handleCommand(const std::string& commandLine);
};

#endif