#include <catch2/catch.hpp>
#include "../include/LettuceDatabase.h"

TEST_CASE("LettuceDatabase is a singleton", "[database]") {
    LettuceDatabase& db1 = LettuceDatabase::getInstance();
    LettuceDatabase& db2 = LettuceDatabase::getInstance();
    REQUIRE(&db1 == &db2);
}

TEST_CASE("LettuceDatabase dump and load always return true (stub)", "[database]") {
    LettuceDatabase& db = LettuceDatabase::getInstance();
    REQUIRE(db.dump("dummyfile.ldb") == true);
    REQUIRE(db.load("dummyfile.ldb") == true);
}