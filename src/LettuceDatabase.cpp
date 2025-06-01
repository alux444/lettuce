#include "../include/LettuceDatabase.h"

#include <string>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>

LettuceDatabase &LettuceDatabase::getInstance()
{
  static LettuceDatabase instance;
  return instance;
}

bool LettuceDatabase::flushAll()
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  keyValueStore.clear();
  listStore.clear();
  hashStore.clear();
  return true;
}

void LettuceDatabase::purgeExpired()
{
  auto now = std::chrono::steady_clock::now();
  for (auto it = expiryMap.begin(); it != expiryMap.end(); )
  {
    if (now > it->second)
    {
      keyValueStore.erase(it->first);
      listStore.erase(it->first);
      hashStore.erase(it->first);
      it = expiryMap.erase(it);
    } else {
      it++;
    }
  }
}

/* Key Value operations*/
void LettuceDatabase::set(const std::string &key, const std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  keyValueStore[key] = value;
}

bool LettuceDatabase::get(const std::string &key, std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto iterator = keyValueStore.find(key);
  if (iterator != keyValueStore.end())
  {
    value = iterator->second;
    return true;
  }
  return false;
}

std::string LettuceDatabase::type(const std::string &key)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  if (keyValueStore.find(key) != keyValueStore.end())
  {
    return "string";
  }
  if (listStore.find(key) != listStore.end())
  {
    return "list";
  }
  if (hashStore.find(key) != hashStore.end())
  {
    return "hash";
  }
  return "none";
}

std::vector<std::string> LettuceDatabase::keys()
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  std::vector<std::string> keys{};
  for (const auto &pair : keyValueStore)
  {
    const std::string &key = pair.first;
    keys.push_back(key);
  }

  for (const auto &pair : listStore)
  {
    const std::string &key = pair.first;
    keys.push_back(key);
  }

  for (const auto &pair : hashStore)
  {
    const std::string &key = pair.first;
    keys.push_back(key);
  }
  return keys;
}

bool LettuceDatabase::del(const std::string &key)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  bool erased = false;
  erased |= keyValueStore.erase(key) > 0;
  erased |= listStore.erase(key) > 0;
  erased |= hashStore.erase(key) > 0;
  return erased;
}

bool LettuceDatabase::expire(const std::string &key, int seconds)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  bool exists = (keyValueStore.find(key) != keyValueStore.end()) ||
                (listStore.find(key) != listStore.end()) ||
                (hashStore.find(key) != hashStore.end());
  if (!exists)
    return false;
  expiryMap[key] = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
  return true;
}

bool LettuceDatabase::rename(const std::string &oldKey, const std::string &newKey)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  bool found = false;
  auto iteratorKv = keyValueStore.find(oldKey);
  if (iteratorKv != keyValueStore.end())
  {
    const std::string &value = iteratorKv->second;
    keyValueStore[newKey] = value;
    keyValueStore.erase(oldKey);
    found = true;
  }
  auto iteratorList = listStore.find(oldKey);
  if (iteratorList != listStore.end())
  {
    const std::vector<std::string> &value = iteratorList->second;
    listStore[newKey] = value;
    listStore.erase(oldKey);
    found = true;
  }
  auto iteratorMap = hashStore.find(oldKey);
  if (iteratorMap != hashStore.end())
  {
    const std::unordered_map<std::string, std::string> &value = iteratorMap->second;
    hashStore[newKey] = value;
    hashStore.erase(oldKey);
    found = true;
  }
  auto iteratorExpiry = expiryMap.find(oldKey);
  if (iteratorExpiry != expiryMap.end())
  {
    const std::chrono::steady_clock::time_point &value = iteratorExpiry->second;
    expiryMap[newKey] = value;
    expiryMap.erase(oldKey);
  }
  return found;
}

bool LettuceDatabase::dump(const std::string &filename)
{
  // use mutex for thread safety
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
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

/* List operations*/
std::vector<std::string> LettuceDatabase::lget(const std::string &key)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  if (listStore.find(key) != listStore.end())
    return listStore[key];
  return {};
}

size_t LettuceDatabase::llen(const std::string &key)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto iterator = listStore.find(key);
  if (iterator != listStore.end())
    return iterator->second.size();
  return 0;
}

void LettuceDatabase::lpush(const std::string &key, const std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  listStore[key].insert(listStore[key].begin(), value);
}

void LettuceDatabase::rpush(const std::string &key, const std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  listStore[key].push_back(value);
}

bool LettuceDatabase::lpop(const std::string &key, std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto iterator = listStore.find(key);
  if (iterator != listStore.end() && !iterator->second.empty())
  {
    value = iterator->second.front();                 // second is the actual vector of list
    iterator->second.erase(iterator->second.begin()); // remove the first value of the vector (front)
    return true;
  }
  return false;
}

bool LettuceDatabase::rpop(const std::string &key, std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto iterator = listStore.find(key);
  if (iterator != listStore.end() && !iterator->second.empty())
  {
    value = iterator->second.back();
    iterator->second.pop_back();
    return true;
  }
  return false;
}

int LettuceDatabase::lrem(const std::string &key, int count, const std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  int removed{0};
  auto iterator = listStore.find(key);
  if (iterator == listStore.end())
    return 0;

  auto &list = iterator->second;

  if (count == 0)
  {
    // remove all occurrences
    auto newEnd = std::remove(list.begin(), list.end(), value); // this actually just reorders in place
    // newEnd is an iterator to the START of the now removed values (all of which after we can get rid of)
    removed = std::distance(newEnd, list.end());
    list.erase(newEnd, list.end());
  }

  if (count > 0)
  {
    // remove elements equal head to tail
    for (auto iter = list.begin(); iter != list.end() && removed < count;)
    {
      if (*iter == value)
      {
        iter = list.erase(iter);
        removed++;
      }
      else
        iter++;
    }
  }

  if (count < 0)
  {
    // remove elements equal tail to head
    for (auto riter = list.rbegin(); riter != list.rend() && removed < count;)
    {
      if (*riter == value)
      {
        // because .erase() only works with a fwd iterator...
        auto forwardIterator = riter.base();           // returns forward iterator, but one element after
        forwardIterator--;                             // so decrement back to our element (that riter is pointing to)
        forwardIterator = list.erase(forwardIterator); // after erasing the value is going to point to the value after the one we erase
        removed++;
        riter = std::reverse_iterator<std::vector<std::string>::iterator>(forwardIterator); // recreate the reverse iterator
      }
      else
        riter++;
    }
  }

  return removed;
}

bool LettuceDatabase::lindex(const std::string &key, int index, std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto iter = listStore.find(key);
  if (iter == listStore.end())
    return false;
  const auto &list = iter->second;
  if (index < 0)
    index = list.size() + index;
  if (index < 0 || static_cast<size_t>(index) >= list.size())
    return false;

  value = list[index];
  return true;
}

bool LettuceDatabase::lset(const std::string &key, int index, const std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto iter = listStore.find(key);
  if (iter == listStore.end())
    return false;
  auto &list = iter->second;
  if (index < 0)
    index = list.size() + index;
  if (index < 0 || static_cast<size_t>(index) >= list.size())
    return false;

  list[index] = value;
  return true;
}

/* Hash operations */
bool LettuceDatabase::hset(const std::string &key, const std::string &field, const std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  hashStore[key][field] = value;
  return true;
}

bool LettuceDatabase::hget(const std::string &key, const std::string &field, std::string &value)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto it = hashStore.find(key);
  if (it != hashStore.end())
  {
    auto f = it->second.find(field);
    if (f != it->second.end())
    {
      std::cerr << "HGET FOUND " << &field << " " << f->second << "\n";
      value = f->second;
      return true;
    }
  }
  return false;
}

bool LettuceDatabase::hexists(const std::string &key, const std::string &field)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto it = hashStore.find(key);
  if (it != hashStore.end())
  {
    auto f = it->second.find(field);
    return f != it->second.end();
  }
  return false;
}

bool LettuceDatabase::hdel(const std::string &key, const std::string &field)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto it = hashStore.find(key);
  if (it != hashStore.end())
  {
    return it->second.erase(field) > 0; // returns 1 on removing something
  }
  return false;
}

std::unordered_map<std::string, std::string> LettuceDatabase::hgetall(const std::string &key)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  if (hashStore.find(key) != hashStore.end())
    return hashStore[key];
  return {};
}

std::vector<std::string> LettuceDatabase::hkeys(const std::string &key)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  std::vector<std::string> fields;
  auto it = hashStore.find(key);
  if (it != hashStore.end())
  {
    for (const auto &[key, _] : it->second)
    {
      fields.push_back(key);
    }
  }
  return fields;
}

std::vector<std::string> LettuceDatabase::hvals(const std::string &key)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  std::vector<std::string> values;
  auto it = hashStore.find(key);
  if (it != hashStore.end())
  {
    for (const auto &[_, value] : it->second)
    {
      values.push_back(value);
    }
  }
  return values;
}

size_t LettuceDatabase::hlen(const std::string &key)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  auto it = hashStore.find(key);
  return it != hashStore.end() ? it->second.size() : 0;
}

bool LettuceDatabase::hmset(const std::string &key, const std::vector<std::pair<std::string, std::string>> &pairs)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
  for (const auto &[field, value] : pairs)
    hashStore[key][field] = value;
  return true;
}

/* Dump files*/
bool LettuceDatabase::load(const std::string &filename)
{
  std::lock_guard<std::mutex> lock(db_mutex);
  purgeExpired();
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