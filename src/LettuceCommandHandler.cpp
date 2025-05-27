#include <../include/LettuceCommandHandler.h>
#include <../include/LettuceDatabase.h>

#include <vector>
#include <sstream>

// RESP (Redis Serialization Protocol) 
// e.g *2\r\n$4\r\nPING\r\n$4\r\nTEST\r\n
// - *2 -> array has 2 elements
// - $4 -> next string has 4 characters
// - PING
// - TEST

std::vector<std::string> parseRespCommand(const std::string& input) {
  std::vector<std::string> tokens;
  if (input.empty()) 
  {
    return tokens;
  }

  if (input[0] != '*')
  {
    std::istringstream iss(input);
    std::string token;
    // fallback to split by whitespace
    while (iss >> token)
    {
      tokens.push_back(token);
    }
    return tokens;
  }

  size_t pos = 0;
  // followed by number of elements
  if (input[pos] != '*')
  {
    return tokens;
  }

  pos++;
  size_t crlf = input.find("\r\n", pos);
  if (crlf == std::string::npos) 
  {
    return tokens; 
  }

  // e.g "*2\r\n" - gets 2
  int numElements = std::stoi(input.substr(pos, crlf-pos));
  pos = crlf + 2; // skip \r\n

  for (int i=0; i<numElements; i++)
  {
    if (pos >= input.size() || input[pos] != '$')
    {
      break; // format error
    }
    pos++; // skip '$'
    crlf = input.find("\r\n", pos);
    if (crlf == std::string::npos) 
    {
      break;
    }

    // e.g "$4\r\n" - gets 4
    int len = std::stoi(input.substr(pos, crlf - pos));
    pos = crlf+2;

    if (pos + len > input.size()) 
    {
      break;
    }
    std::string token = input.substr(pos, len);
    tokens.push_back(token);
    pos += len+2; // skip token and \r\n
  }

  return tokens;
}

LettuceCommandHandler::LettuceCommandHandler() {}

std::string LettuceCommandHandler::handleCommand(const std::string& commandLine) {
  std::vector<std::string> tokens = parseRespCommand(commandLine);
  
  if (tokens.empty()) {
    return "-ERR empty command\r\n";
  }

  std::string command = tokens[0];
  std::transform(command.begin(), command.end(), command.begin(), ::toupper);
  std::ostringstream response;
  // return the command
  // TODO: connect to database
  // LettuceDatabase& db = LettuceDatabase.getInstance();

  // TODO: check commands
  if (command == "PING")
  {
    response << "+PONG\r\n";
  } else {
    response << "-ERR: Unknown command\r\n";
  }
  
  return response.str();
}