
#include <../include/LettuceCommandHandler.h>
#include <../include/LettuceDatabase.h>

#include <string>
#include <sstream>
#include <vector>

std::string handlePing(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "+PONG\r\n";
}

std::string handleEcho(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 2)
  {
    return "-ERR: ECHO requires an argument\r\n";
  }
  return "+" + vector[1] + "\r\n";
}

std::string handleFlushAll(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  db.flushAll();
  return "+OK\r\n";
}

std::string handleSet(const std::vector<std::string> &vector, LettuceDatabase &db)
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

std::string handleGet(const std::vector<std::string> &vector, LettuceDatabase &db)
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

std::string handleKeys(const std::vector<std::string> &vector, LettuceDatabase &db)
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

std::string handleType(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 2)
  {
    return "-ERR: TYPE requires a KEY argument\r\n";
  }
  const std::string &key = vector[1];
  const std::string &type = db.type(key);
  return "+" + type + "\r\n";
}

std::string handleDel(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  if (vector.size() < 2)
  {
    return "-ERR: DEL requires a KEY argument\r\n";
  }
  const std::string &key = vector[1];
  bool deleted = db.del(key);
  return ":" + std::to_string(deleted ? 1 : 0) + "\r\n";
}

std::string handleExpire(const std::vector<std::string> &vector, LettuceDatabase &db)
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

std::string handleRename(const std::vector<std::string> &vector, LettuceDatabase &db)
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

std::string handleLlen(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "";
}

std::string handleLpush(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "";
}

std::string handleRpush(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "";
}

std::string handleLpop(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "";
}

std::string handleRpop(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "";
}

std::string handleLrem(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "";
}

std::string handleLindex(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "";
}

std::string handleLset(const std::vector<std::string> &vector, LettuceDatabase &db)
{
  return "";
}