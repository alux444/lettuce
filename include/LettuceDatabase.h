#ifndef LETTUCE_DATABASE_H
#define LETTUCE_DATABASE_H

#include <string>
class LettuceDatabase
{
public:
  static LettuceDatabase& getInstance(); // singleton

  bool dump(const std::string &filename);
  bool load(const std::string &filename);

private:
  LettuceDatabase() = default;                                  // default constructor
  ~LettuceDatabase() = default;                                 // default destructor
  LettuceDatabase(const LettuceDatabase &) = delete;            // deletes copy constructor
  LettuceDatabase &operator=(const LettuceDatabase &) = delete; // deletes assignment operator
};

#endif