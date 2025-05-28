#ifndef LETTUCE_DATABASE_H
#define LETTUCE_DATABASE_H

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
class LettuceDatabase
{
public:
  std::unordered_map<std::string, std::string> keyValueStore;
  std::unordered_map<std::string, std::vector<std::string>> listStore;
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> hashStore;

  static LettuceDatabase &getInstance(); // singleton

  bool dump(const std::string &filename);
  bool load(const std::string &filename);

private:
  std::mutex db_mutex;
  LettuceDatabase() = default;                                  // default constructor
  ~LettuceDatabase() = default;                                 // default destructor
  LettuceDatabase(const LettuceDatabase &) = delete;            // deletes copy constructor
  LettuceDatabase &operator=(const LettuceDatabase &) = delete; // deletes assignment operator
};

#endif