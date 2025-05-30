#include "../include/LettuceDatabase.h"

#include <string>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <fstream>
#include <sstream>

LettuceDatabase &LettuceDatabase::getInstance()
{
  static LettuceDatabase instance;
  return instance;
}

bool LettuceDatabase::flushAll()
{
  std::lock_guard<std::mutex> lock(db_mutex);
  keyValueStore.clear();
  listStore.clear();
  hashStore.clear();
  return true;
}

void LettuceDatabase::set(const std::string &key, const std::string &value)
{
  return;
}

bool LettuceDatabase::get(const std::string &key, const std::string &value) const
{
  return true;
}

std::string LettuceDatabase::type(const std::string &key) const
{
  return "";
}

std::vector<std::string> LettuceDatabase::keys() const
{
  std::vector<std::string> v{};
  return v;
}

bool LettuceDatabase::del(const std::string &key)
{
  return true;
}

bool LettuceDatabase::expire(const std::string& key, int seconds)
{
  return true;
}

bool LettuceDatabase::rename(const std::string &oldKey, const std::string &newKey)
{
  return true;
}

bool LettuceDatabase::dump(const std::string &filename)
{
  // use mutex for thread safety
  std::lock_guard<std::mutex> lock(db_mutex);
  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs)
    return false;

  for (const auto &keyValue : keyValueStore)
  {
    ofs << "K " << keyValue.first << " " << keyValue.second << "\n";
  }

  for (const auto &keyList : listStore)
  {
    ofs << "L " << keyList.first;
    for (const auto &item : keyList.second)
      ofs << " " << item;
    ofs << "\n";
  }

  for (const auto &keyMap : hashStore)
  {
    ofs << "H " << keyMap.first;
    for (const auto &mapKeyValue : keyMap.second)
      ofs << " " << mapKeyValue.first << ":" << mapKeyValue.second;
    ofs << "\n";
  }

  return true;
}

bool LettuceDatabase::load(const std::string &filename)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  std::ifstream ifs(filename, std::ios::binary);
  if (!ifs)
    return false;

  keyValueStore.clear();
  listStore.clear();
  hashStore.clear();

  std::string line;
  while (std::getline(ifs, line))
  {
    std::istringstream iss(line);
    char type;
    iss >> type;
    if (type == 'K')
    {
      std::string key, value;
      iss >> key >> value;
      keyValueStore[key] = value;
    }

    if (type == 'L')
    {
      std::string key;
      iss >> key;
      std::string item;
      std::vector<std::string> values;
      while (iss >> item)
        values.push_back(item);
      listStore[key] = values;
    }

    if (type == 'H')
    {
      std::string key;
      iss >> key;
      std::string pair;
      std::unordered_map<std::string, std::string> map;
      while (iss >> pair)
      {
        int position = pair.find(':');
        if (position != std::string::npos)
        {
          std::string key = pair.substr(0, position);
          std::string value = pair.substr(position + 1);
          map[key] = value;
        }
        hashStore[key] = map;
      }
    }
  }

  return true;
}