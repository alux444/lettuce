#include <catch2/catch.hpp>
#include "../include/LettuceDatabase.h"
#include "test_utils.h"

#include <cstdio> // for std::remove

TEST_CASE("LettuceDatabase is a singleton", "[database]") {
    LettuceDatabase& db1 = LettuceDatabase::getInstance();
    LettuceDatabase& db2 = LettuceDatabase::getInstance();
    REQUIRE(&db1 == &db2);

    cleanup();
}

TEST_CASE("LettuceDatabase dump and load for key-value, list, and hash", "[database]") {
    LettuceDatabase& db = LettuceDatabase::getInstance();

    db.keyValueStore["foo"] = "bar";
    db.listStore["mylist"] = {"a", "b", "c"};
    db.hashStore["myhash"] = {{"field1", "val1"}, {"field2", "val2"}};

    REQUIRE(db.dump(test_db_filename) == true);

    db.keyValueStore.clear();
    db.listStore.clear();
    db.hashStore.clear();

    REQUIRE(db.load(test_db_filename) == true);

    REQUIRE(db.keyValueStore["foo"] == "bar");

    REQUIRE(db.listStore["mylist"].size() == 3);
    REQUIRE(db.listStore["mylist"][0] == "a");
    REQUIRE(db.listStore["mylist"][2] == "c");

    REQUIRE(db.hashStore["myhash"]["field1"] == "val1");
    REQUIRE(db.hashStore["myhash"]["field2"] == "val2");

    cleanup();
}

TEST_CASE("LettuceDatabase flushall clears all stores", "[database]") {
    LettuceDatabase& db = LettuceDatabase::getInstance();

    db.keyValueStore["foo"] = "bar";
    db.listStore["mylist"] = {"a", "b", "c"};
    db.hashStore["myhash"] = {{"field1", "val1"}, {"field2", "val2"}};

    REQUIRE(db.flushAll() == true);

    REQUIRE(db.keyValueStore.empty());
    REQUIRE(db.listStore.empty());
    REQUIRE(db.hashStore.empty());

    cleanup();
}