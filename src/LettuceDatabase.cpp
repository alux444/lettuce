#include "../include/LettuceDatabase.h"

#include <string>
#include <mutex>
#include <fstream>
#include <sstream>

LettuceDatabase& LettuceDatabase::getInstance() {
  static LettuceDatabase instance;
  return instance;
}

bool LettuceDatabase::dump(const std::string& filename) {
  // use mutex for thread safety
  std::lock_guard<std::mutex> lock(db_mutex);
  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs)
    return false;

  for (const auto& keyValue : keyValueStore) 
  {
    ofs << "K" << keyValue.first << " " << keyValue.second << "\n";
  }

  for (const auto& keyList : listStore)
  {
    ofs << "L " << keyList.first;
    for (const auto& item : keyList.second)
      ofs << " " << item;
    ofs << "\n";
  }

  for (const auto& keyMap : hashStore)
  {
    ofs << "H " << keyMap.first;
    for (const auto& mapKeyValue : keyMap.second)
      ofs << " " << mapKeyValue.first << ":" << mapKeyValue.second; 
    ofs << "\n";
  }

  return true;
}

bool LettuceDatabase::load(const std::string& filename) {
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

    }

    if (type == 'L')
    {

    }

    if (type == 'M')
    {

    }
  }

  return true;
}