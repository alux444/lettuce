#include <catch2/catch.hpp>
#include "../include/LettuceDatabase.h"
#include "test_utils.h"

#include <cstdio> // for std::remove

TEST_CASE("LettuceDatabase is a singleton", "[database]")
{
    LettuceDatabase &db1 = LettuceDatabase::getInstance();
    LettuceDatabase &db2 = LettuceDatabase::getInstance();
    REQUIRE(&db1 == &db2);

    cleanup();
}

TEST_CASE("LettuceDatabase dump and load for key-value, list, and hash", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();

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

TEST_CASE("LettuceDatabase flushall clears all stores", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();

    db.keyValueStore["foo"] = "bar";
    db.listStore["mylist"] = {"a", "b", "c"};
    db.hashStore["myhash"] = {{"field1", "val1"}, {"field2", "val2"}};

    REQUIRE(db.flushAll() == true);

    REQUIRE(db.keyValueStore.empty());
    REQUIRE(db.listStore.empty());
    REQUIRE(db.hashStore.empty());

    cleanup();
}

TEST_CASE("LettuceDatabase set and get", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.set("alpha", "beta");
    std::string value;
    REQUIRE(db.get("alpha", value));
    REQUIRE(value == "beta");

    cleanup();
}

TEST_CASE("LettuceDatabase del removes keys", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.set("foo", "bar");
    REQUIRE(db.del("foo"));
    std::string value;
    REQUIRE_FALSE(db.get("foo", value));

    cleanup();
}

TEST_CASE("LettuceDatabase keys returns all keys", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.set("a", "1");
    db.listStore["b"] = {"x", "y"};
    db.hashStore["c"] = {{"k", "v"}};

    auto keys = db.keys();
    REQUIRE(std::find(keys.begin(), keys.end(), "a") != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), "b") != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), "c") != keys.end());

    cleanup();
}

TEST_CASE("LettuceDatabase type returns correct type", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.set("str", "val");
    db.listStore["lst"] = {"1", "2"};
    db.hashStore["hsh"] = {{"f", "v"}};

    REQUIRE(db.type("str") == "string");
    REQUIRE(db.type("lst") == "list");
    REQUIRE(db.type("hsh") == "hash");
    REQUIRE(db.type("nope") == "none");

    cleanup();
}

TEST_CASE("LettuceDatabase expire and rename", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.set("foo", "bar");
    REQUIRE(db.expire("foo", 1));
    REQUIRE(db.rename("foo", "baz"));
    std::string value;
    REQUIRE_FALSE(db.get("foo", value));
    REQUIRE(db.get("baz", value));
    REQUIRE(value == "bar");

    cleanup();
}