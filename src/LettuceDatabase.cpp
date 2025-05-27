#include "../include/LettuceDatabase.h"

#include <string>

LettuceDatabase& LettuceDatabase::getInstance() {
  static LettuceDatabase instance;
  return instance;
}

bool LettuceDatabase::dump(const std::string& filename) {
  return true;
}

bool LettuceDatabase::load(const std::string& filename) {
  return true;
}