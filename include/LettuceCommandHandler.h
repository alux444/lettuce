#ifndef LETTUCE_COMMAND_HANDLER_H
#define LETTUCE_COMMAND_HANDLER_H

#include <string>
#include <vector>

class LettuceCommandHandler
{
public:
  LettuceCommandHandler();
  std::string handleCommand(const std::string& commandLine);
};

std::vector<std::string> parseRespCommand(const std::string& input);

#endif