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
  void set(const std::string& key, const std::string& value);
  bool get(const std::string& key, const std::string& value) const;
  std::vector<std::string> keys() const;
  std::string type(const std::string& key) const;
  bool del(const std::string& key);
  bool expire(const std::string& key, int seconds);
  bool rename(const std::string& oldKey, const std::string& newKey);

private:
  std::mutex db_mutex;
  LettuceDatabase() = default;                                  // default constructor
  ~LettuceDatabase() = default;                                 // default destructor
  LettuceDatabase(const LettuceDatabase &) = delete;            // deletes copy constructor
  LettuceDatabase &operator=(const LettuceDatabase &) = delete; // deletes assignment operator
};

#endif