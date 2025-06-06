
#include <../include/LettuceCommandHandler.h>
#include <../include/LettuceDatabase.h>

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

std::string handlePing(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  return "+PONG\r\n";
}

std::string handleEcho(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: ECHO requires an argument\r\n";
  }
  return "+" + tokens[1] + "\r\n";
}

std::string handleFlushAll(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  db.flushAll();
  return "+OK\r\n";
}

/* Key value related operations */
std::string handleSet(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: SET expects 2 arguments - key and value\r\n";
  }
  const std::string &key = tokens[1];
  const std::string &value = tokens[2];
  db.set(key, value);
  return "+OK\r\n";
}

std::string handleGet(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: GET requires a key\r\n";
  }
  const std::string &key = tokens[1];
  std::string value;
  if (db.get(key, value))
  {
    return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
  }
  return "$-1\r\n";
}

std::string handleKeys(const std::vector<std::string> &tokens, LettuceDatabase &db)
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

std::string handleType(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: TYPE requires a KEY argument\r\n";
  }
  const std::string &key = tokens[1];
  const std::string &type = db.type(key);
  return "+" + type + "\r\n";
}

std::string handleDel(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: DEL requires a KEY argument\r\n";
  }
  const std::string &key = tokens[1];
  bool deleted = db.del(key);
  return ":" + std::to_string(deleted ? 1 : 0) + "\r\n";
}

std::string handleExpire(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: EXPIRE requires a KEY and TIME in seconds\r\n";
  }
  const std::string &key = tokens[1];
  int timeInSeconds = std::stoi(tokens[2]);
  bool expired = db.expire(key, timeInSeconds);
  return ":" + std::to_string(expired ? 1 : 0) + "\r\n";
}

std::string handleRename(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: RENAME requires an OLD KEY VALUE and NEW KEY VALUE\r\n";
  }
  const std::string &oldKey = tokens[1];
  const std::string &newKey = tokens[2];
  bool renamed = db.rename(oldKey, newKey);
  return ":" + std::to_string(renamed ? 1 : 0) + "\r\n";
}

/* List related operations */
std::string handleLget(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: LGET requires a KEY\r\n";
  }
  const std::string &key = tokens[1];
  std::vector<std::string> list = db.lget(key);
  std::ostringstream oss;
  oss << "*" << list.size() << "\r\n";
  for (const auto &value : list)
    oss << "$" << value.size() << "\r\n"
        << value << "\r\n";
  return oss.str();
}

std::string handleLlen(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: LLEN requires a KEY\r\n";
  }
  const std::string &key = tokens[1];
  size_t len = db.llen(key);
  return ":" + std::to_string(len) + "\r\n";
}

std::string handleLpush(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: LPUSH requires a KEY and VALUE\r\n";
  }
  const std::string &key = tokens[1];
  const std::string &value = tokens[2];
  db.lpush(key, value);
  size_t len = db.llen(key);
  return ":" + std::to_string(len) + "\r\n";
}

std::string handleRpush(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: RPUSH requires a KEY and VALUE\r\n";
  }
  const std::string &key = tokens[1];
  const std::string &value = tokens[2];
  db.rpush(key, value);
  size_t len = db.llen(key);
  return ":" + std::to_string(len) + "\r\n";
}

std::string handleLpop(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: LPOP requires a KEY\r\n";
  }
  const std::string &key = tokens[1];
  std::string value{};
  if (db.lpop(key, value))
    return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
  return "$-1\r\n";
}

std::string handleRpop(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: RPOP requires a KEY\r\n";
  }
  const std::string &key = tokens[1];
  std::string value{};
  if (db.rpop(key, value))
    return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
  return "$-1\r\n";
}

std::string handleLrem(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 4)
  {
    return "-ERR: LREM requires a KEY, COUNT and VALUE\r\n";
  }
  try
  {
    int count = std::stoi(tokens[2]);
    const std::string &key = tokens[1];
    const std::string &value = tokens[3];
    int removed = db.lrem(key, count, value);
    return ":" + std::to_string(removed) + "\r\n";
  }
  catch (const std::exception &)
  {
    return "-ERR: Invalid count value\r\n";
  }
}

std::string handleLindex(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: LINDEX requires a KEY and INDEX\r\n";
  }
  try
  {
    int index = std::stoi(tokens[2]);
    const std::string &key = tokens[1];
    std::string value{};
    if (db.lindex(key, index, value))
      return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
    return "$-1\r\n";
  }
  catch (const std::exception &)
  {
    return "-ERR: Invalid index value\r\n";
  }
}

std::string handleLset(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 4)
  {
    return "-ERR: LSET requires a KEY, INDEX and VALUE\r\n";
  }
  try
  {
    int index = std::stoi(tokens[2]);
    const std::string &key = tokens[1];
    const std::string &value = tokens[3];
    if (db.lset(key, index, value))
      return "+OK\r\n";
    return "-ERR: Index out of range\r\n";
  }
  catch (const std::exception &)
  {
    return "-ERR: Invalid index value\r\n";
  }
}

/* Hash operations */
std::string handleHset(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 4)
  {
    return "-ERR: HSET requires a KEY, FIELD and VALUE\r\n";
  }
  const std::string key = tokens[1];
  const std::string field = tokens[2];
  const std::string value = tokens[3];
  db.hset(key, field, value);
  return ":1\r\n";
}

std::string handleHget(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: HSET requires a KEY and FIELD\r\n";
  }
  const std::string key = tokens[1];
  const std::string field = tokens[2];
  std::string value;
  if (db.hget(key, field, value))
    return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
  return "$-1\r\n";
}

std::string handleHexists(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: HEXISTS requires a KEY and FIELD\r\n";
  }
  const std::string key = tokens[1];
  const std::string field = tokens[2];
  bool exists = (db.hexists(key, field));
  return ":" + std::to_string(exists ? 1 : 0) + "\r\n";
}

std::string handleHdel(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 3)
  {
    return "-ERR: HDEL requires a KEY and FIELD\r\n";
  }
  const std::string key = tokens[1];
  const std::string field = tokens[2];
  bool deleted = db.hdel(key, field);
  return ":" + std::to_string(deleted ? 1 : 0) + "\r\n";
}

std::string handleHgetall(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: HGETALL requires a KEY\r\n";
  }
  const std::string key = tokens[1];
  auto hash = db.hgetall(key);
  std::ostringstream oss;
  oss << "*" << hash.size() * 2 << "\r\n"; // two strings per hash
  for (const auto &[key, value] : hash)
  {
    oss << "$" << key.size() << "\r\n"
        << key << "\r\n";
    oss << "$" << value.size() << "\r\n"
        << value << "\r\n";
  }
  return oss.str();
}

std::string handleHkeys(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: HGETALL requires a KEY\r\n";
  }
  const std::string key = tokens[1];
  auto keys = db.hkeys(key);
  std::ostringstream oss;
  oss << "*" << keys.size() << "\r\n";
  for (const auto &key : keys)
    oss << "$" << key.size() << "\r\n"
        << key << "\r\n";
  return oss.str();
}

std::string handleHvals(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: HVALS requires a KEY\r\n";
  }
  const std::string key = tokens[1];
  auto values = db.hvals(key);
  std::ostringstream oss;
  oss << "*" << values.size() << "\r\n";
  for (const auto &value : values)
    oss << "$" << value.size() << "\r\n"
        << value << "\r\n";
  return oss.str();
}

std::string handleHlen(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 2)
  {
    return "-ERR: HLEN requires a KEY\r\n";
  }
  const std::string key = tokens[1];
  size_t len = db.hlen(key);
  std::cerr << "GOT LEN " << len << "\n";
  return ":" + std::to_string(len) + "\r\n";
}

std::string handleHmset(const std::vector<std::string> &tokens, LettuceDatabase &db)
{
  if (tokens.size() < 4 || (tokens.size() % 2 == 1))
  {
    return "-ERR: HMSET requires a KEY following by FIELD and VALUE\r\n";
  }
  const std::string key = tokens[1];
  std::vector<std::pair<std::string, std::string>> fieldValues;
  for (size_t i = 2; i < tokens.size(); i += 2)
  {
    const std::string field = tokens[i];
    const std::string value = tokens[i + 1];
    fieldValues.emplace_back(field, value);
  }
  db.hmset(key, fieldValues);
  return "+OK\r\n";
}