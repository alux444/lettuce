#ifndef LETTUCE_DATABASE_H
#define LETTUCE_DATABASE_H

#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <unordered_map>
class LettuceDatabase
{
public:
  std::unordered_map<std::string, std::string> keyValueStore;
  std::unordered_map<std::string, std::vector<std::string>> listStore;
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> hashStore;
  std::unordered_map<std::string, std::chrono::steady_clock::time_point> expiryMap;

  static LettuceDatabase &getInstance(); // singleton

  bool dump(const std::string &filename);
  bool load(const std::string &filename);

  bool flushAll();

  // key values
  void set(const std::string &key, const std::string &value);
  bool get(const std::string &key, std::string &value);
  std::vector<std::string> keys();
  std::string type(const std::string &key);
  bool del(const std::string &key);
  bool expire(const std::string &key, int seconds);
  bool rename(const std::string &oldKey, const std::string &newKey);

  // list
  size_t llen(const std::string &key);
  void lpush(const std::string &key, const std::string &value);
  void rpush(const std::string &key, const std::string &value);
  bool lpop(const std::string &key, std::string &value);
  bool rpop(const std::string &key, std::string &value);
  int lrem(const std::string &key, int count, const std::string &value);
  bool lindex(const std::string &key, int index, std::string &value);
  bool lset(const std::string &key, int index, const std::string &value);

  // hashes
  bool hset(const std::string &key, const std::string &field, const std::string &value);
  bool hget(const std::string &key, const std::string &field, std::string &value);
  bool hexists(const std::string &key, const std::string &field);
  bool hdel(const std::string &key, const std::string &field);
  std::unordered_map<std::string, std::string> hgetall(const std::string &key);
  std::vector<std::string> hkeys(const std::string &key);
  std::vector<std::string> hvals(const std::string &key);
  size_t hlen(const std::string &key);
  bool hmset(const std::string &key, const std::vector<std::pair<std::string, std::string>> &pairs);

private:
  std::mutex db_mutex;
  LettuceDatabase() = default;                                  // default constructor
  ~LettuceDatabase() = default;                                 // default destructor
  LettuceDatabase(const LettuceDatabase &) = delete;            // deletes copy constructor
  LettuceDatabase &operator=(const LettuceDatabase &) = delete; // deletes assignment operator
};

#endif