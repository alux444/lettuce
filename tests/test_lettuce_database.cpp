#include <catch2/catch.hpp>
#include "../include/LettuceDatabase.h"

#include <cstdio> // for std::remove

TEST_CASE("LettuceDatabase is a singleton", "[database]") {
    LettuceDatabase& db1 = LettuceDatabase::getInstance();
    LettuceDatabase& db2 = LettuceDatabase::getInstance();
    REQUIRE(&db1 == &db2);
}

TEST_CASE("LettuceDatabase dump and load for key-value, list, and hash", "[database]") {
    LettuceDatabase& db = LettuceDatabase::getInstance();

    // Insert data
    db.keyValueStore["foo"] = "bar";
    db.listStore["mylist"] = {"a", "b", "c"};
    db.hashStore["myhash"] = {{"field1", "val1"}, {"field2", "val2"}};

    // Dump to file
    std::string filename = "dump.ldb";
    REQUIRE(db.dump(filename) == true);

    // Clear in-memory data
    db.keyValueStore.clear();
    db.listStore.clear();
    db.hashStore.clear();

    // Load from file
    REQUIRE(db.load(filename) == true);

    // Check key-value
    REQUIRE(db.keyValueStore["foo"] == "bar");

    // Check list
    REQUIRE(db.listStore["mylist"].size() == 3);
    REQUIRE(db.listStore["mylist"][0] == "a");
    REQUIRE(db.listStore["mylist"][2] == "c");

    // Check hash
    REQUIRE(db.hashStore["myhash"]["field1"] == "val1");
    REQUIRE(db.hashStore["myhash"]["field2"] == "val2");

    // Clean up
    std::remove(filename.c_str());
}