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

static std::string handlePing(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "+PONG\r\n";
}

static std::string handleEcho(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 2)
  {
    return "-ERR: ECHO requires an argument\r\n";
  }
  return "+" + vector[1] + "\r\n";
}

static std::string handleFlushAll(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  db.flushAll();
  return "+OK\r\n";
}

static std::string handleSet(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 3)
  {
    return "-ERR: SET expects 2 arguments - key and value\r\n";
  }
  const std::string &key = vector[1];
  const std::string &value = vector[2];
  db.set(key, value);
  return "+OK\r\n";
}

static std::string handleGet(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 2)
  {
    return "-ERR: GET requires a key\r\n";
  }
  const std::string &key = vector[1];
  std::string value;
  if (db.get(key, value))
  {
    return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
  }
  return "$-1\r\n";
}

static std::string handleKeys(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  std::vector<std::string> allKeys = db.keys();
  std::ostringstream response;
  response << "*" << allKeys.size() << "\r\n";
  for (const auto &key : allKeys)
  {
    response << "$" << key.size() << "\r\n"
             << key << "\r\n";
  }
  return response.str();
}

static std::string handleType(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 2)
  {
    return "-ERR: TYPE requires a KEY argument\r\n";
  }
  const std::string &key = vector[1];
  const std::string &type = db.type(key);
  return "+" + type + "\r\n";
}

static std::string handleDel(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 2)
  {
    return "-ERR: DEL requires a KEY argument\r\n";
  }
  const std::string &key = vector[1];
  bool deleted = db.del(key);
  return ":" + std::to_string(deleted ? 1 : 0) + "\r\n";
}

static std::string handleExpire(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 3)
  {
    return "-ERR: EXPIRE requires a KEY and TIME in seconds\r\n";
  }
  const std::string &key = vector[1];
  int timeInSeconds = std::stoi(vector[2]);
  bool expired = db.expire(key, timeInSeconds);
  return ":" + std::to_string(expired ? 1 : 0) + "\r\n";
}

static std::string handleRename(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 3)
  {
    return "-ERR: RENAME requires an OLD KEY VALUE and NEW KEY VALUE\r\n";
  }
  const std::string &oldKey = vector[1];
  const std::string &newKey = vector[2];
  bool renamed = db.rename(oldKey, newKey);
  return ":" + std::to_string(renamed ? 1 : 0) + "\r\n";
}

std::string LettuceCommandHandler::handleCommand(const std::string &commandLine)
{
  std::vector<std::string> tokens = parseRespCommand(commandLine);

  if (tokens.empty())
  {
    return "-ERR: empty command\r\n";
  }

  std::string command = tokens[0];
  std::cout << "Got command: " << command << std::endl;
  std::transform(command.begin(), command.end(), command.begin(), ::toupper);
  std::ostringstream response;

  LettuceDatabase &db = LettuceDatabase::getInstance();

  if (command == "PING")
  {
    response << handlePing(tokens, db);
  }
  else if (command == "ECHO")
  {
    response << handleEcho(tokens, db);
  }
  else if (command == "FLUSHALL")
  {
    response << handleFlushAll(tokens, db);
  }
  else if (command == "SET")
  {
    response << handleSet(tokens, db);
  }
  else if (command == "GET")
  {
    response << handleGet(tokens, db);
  }
  else if (command == "KEYS")
  {
    response << handleKeys(tokens, db);
  }
  else if (command == "TYPE")
  {
    response << handleType(tokens, db);
  }
  else if (command == "DEL")
  {
    response << handleDel(tokens, db);
  }
  else if (command == "EXPIRE")
  {
    response << handleExpire(tokens, db);
  }
  else if (command == "RENAME")
  {
    response << handleRename(tokens, db);
  }
  else
  {
    response << "-ERR: Unknown command\r\n";
  }

  db.dump("dump.ldb");

  return response.str();
}