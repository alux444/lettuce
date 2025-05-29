#include <../include/LettuceCommandHandler.h>
#include <../include/LettuceDatabase.h>

#include <vector>
#include <sstream>
#include <iostream>

// RESP (Redis Serialization Protocol)
// e.g *2\r\n$4\r\nPING\r\n$4\r\nTEST\r\n
// - *2 -> array has 2 elements
// - $4 -> next string has 4 characters
// - PING
// - TEST

std::vector<std::string> parseRespCommand(const std::string &input)
{
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
  int numElements = std::stoi(input.substr(pos, crlf - pos));
  pos = crlf + 2; // skip \r\n

  for (int i = 0; i < numElements; i++)
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
    pos = crlf + 2;

    if (pos + len > input.size())
    {
      break;
    }
    std::string token = input.substr(pos, len);
    tokens.push_back(token);
    pos += len + 2; // skip token and \r\n
  }

  return tokens;
}

LettuceCommandHandler::LettuceCommandHandler() {}

std::string LettuceCommandHandler::handleCommand(const std::string &commandLine)
{
  std::vector<std::string> tokens = parseRespCommand(commandLine);

  if (tokens.empty())
  {
    return "-ERR empty command\r\n";
  }
  
  std::string command = tokens[0];
  std::cout << "Got command: " << command << std::endl;
  std::transform(command.begin(), command.end(), command.begin(), ::toupper);
  std::ostringstream response;

  LettuceDatabase &db = LettuceDatabase::getInstance();

  if (command == "PING")
  {
    response << "+PONG\r\n";
  }
  else if (command == "ECHO")
  {
    if (tokens.size() < 2)
    {
      response << "-ERR: ECHO requires an argument\r\n";
    }
    else
    {
      response << "+" << tokens[1];
    }
  }
  else if (command == "FLUSHALL")
  {
    db.flushAll();
  }
  else if (command == "SET")
  {
    if (tokens.size() < 3)
    {
      response << "-ERR: SET expects 2 arguments - key and value\r\n";
    }
    else
    {
      const std::string& key = tokens[1];
      const std::string& value = tokens[2];
      db.set(key, value);
      response << "+OK";
    }
  }
  else if (command == "GET")
  {
    if (tokens.size() < 2)
    {
      response << "-ERR: GET requires a key\r\n";
    }
    else
    {
      const std::string& key = tokens[1];
      const std::string& value = tokens[2];
      if (db.get(key, value))
      {
        response << "$" << value.size() << "\r\n" << value << "\r\n";
      }
      else
      {
        response << "$-1\r\n";
      }
    }
  }
  else if (command == "KEYS")
  {
    std::vector<std::string> allKeys = db.keys();
    response << "*" << allKeys.size() << "\r\n";
    for (const auto& key : allKeys)
    {
      response << "$" << key.size() << "/r/n" << key << "/r/n";
    }
  }
  else if (command == "TYPE")
  {
    if (tokens.size() < 2)
    {
      response << "-ERR: TYPE requires a KEY argument\r\n";
    }
    else
    {
      const std::string& key = tokens[1];
      const std::string& type = db.type(key);
      response << "+" << type << "\r\n";
    }
  }
  else if (command == "DEL")
  {
    if (tokens.size() < 2)
    {
      response << "-ERR: DEL requires a KEY argument\r\n";
    }
    else
    {
      const std::string& key = tokens[1];
      bool deleted = db.del(key);
      response << ":" << (deleted ? 1 : 0) << "\r\n";
    }
  }
  else if (command == "EXPIRE")
  {
    if (tokens.size() < 3)
    {
      response << "-ERR: EXPIRE requires a KEY and TIME in seconds\r\n";
    }
    else
    {
      const std::string& key = tokens[1];
      const int& timeInSeconds = std::stoi(tokens[2]);
      bool expired = db.expire(key, timeInSeconds);
      response << ":" << (expired ? 1 : 0) << "\r\n";
    }
  }
  else if (command == "RENAME")
  {
    if (tokens.size() < 3)
    {
      response << "ERR: RENAME requires an OLD KEY VALUE and NEW KEY VALUE\r\n";
    }
    else
    {
      const std::string& oldKey = tokens[1];
      const std::string& newKey = tokens[2];
      bool renamed = db.rename(oldKey, newKey);
      response << ":" << (renamed ? 1 : 0) << "\r\n";
    }
  }
  else
  {
    response << "-ERR: Unknown command\r\n";
  }

  db.dump("dump.ldb");

  return response.str();
}