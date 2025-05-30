#include "test_utils.h"
#include <cstdio>
#include <string>

std::string test_db_filename = "dump.ldb";

void cleanup() {
    std::remove(test_db_filename.c_str());
}