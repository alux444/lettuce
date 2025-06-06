#include <catch2/catch.hpp>
#include "../include/LettuceDatabase.h"
#include "test_utils.h"

#include <cstdio> // for std::remove
#include <chrono>
#include <thread>

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

TEST_CASE("LettuceDatabase llen returns correct length", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.lpush("mylist", "a");
    db.rpush("mylist", "b");
    db.rpush("mylist", "c");
    REQUIRE(db.llen("mylist") == 3);

    cleanup();
}

TEST_CASE("LettuceDatabase lpop and rpop remove elements", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.rpush("mylist", "x");
    db.rpush("mylist", "y");
    db.rpush("mylist", "z");

    std::string val;
    REQUIRE(db.lpop("mylist", val));
    REQUIRE(val == "x");
    REQUIRE(db.rpop("mylist", val));
    REQUIRE(val == "z");
    REQUIRE(db.llen("mylist") == 1);

    cleanup();
}

TEST_CASE("LettuceDatabase lrem removes correct elements", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.rpush("mylist", "a");
    db.rpush("mylist", "b");
    db.rpush("mylist", "a");
    db.rpush("mylist", "a");
    db.rpush("mylist", "c");

    // Remove all "a"
    REQUIRE(db.lrem("mylist", 0, "a") == 3);
    REQUIRE(db.llen("mylist") == 2);

    cleanup();
}

TEST_CASE("LettuceDatabase lindex gets correct element", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.rpush("mylist", "foo");
    db.rpush("mylist", "bar");
    db.rpush("mylist", "baz");

    std::string val;
    REQUIRE(db.lindex("mylist", 1, val));
    REQUIRE(val == "bar");
    REQUIRE(db.lindex("mylist", -1, val));
    REQUIRE(val == "baz");

    cleanup();
}

TEST_CASE("LettuceDatabase lset sets correct element", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.rpush("mylist", "one");
    db.rpush("mylist", "two");
    db.rpush("mylist", "three");

    REQUIRE(db.lset("mylist", 1, "TWO"));
    std::string val;
    REQUIRE(db.lindex("mylist", 1, val));
    REQUIRE(val == "TWO");

    cleanup();
}

TEST_CASE("LettuceDatabase hset and hget work", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    REQUIRE(db.hset("myhash", "field1", "val1"));
    std::string value;
    REQUIRE(db.hget("myhash", "field1", value));
    REQUIRE(value == "val1");

    cleanup();
}

TEST_CASE("LettuceDatabase hexists and hdel work", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.hset("myhash", "field1", "val1");
    REQUIRE(db.hexists("myhash", "field1"));
    REQUIRE(db.hdel("myhash", "field1"));
    REQUIRE_FALSE(db.hexists("myhash", "field1"));

    cleanup();
}

TEST_CASE("LettuceDatabase hgetall returns all fields and values", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.hset("myhash", "field1", "val1");
    db.hset("myhash", "field2", "val2");
    auto all = db.hgetall("myhash");
    REQUIRE(all.size() == 2);
    REQUIRE(all["field1"] == "val1");
    REQUIRE(all["field2"] == "val2");

    cleanup();
}

TEST_CASE("LettuceDatabase hkeys and hvals return correct lists", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.hset("myhash", "field1", "val1");
    db.hset("myhash", "field2", "val2");
    auto keys = db.hkeys("myhash");
    auto vals = db.hvals("myhash");
    REQUIRE(std::find(keys.begin(), keys.end(), "field1") != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), "field2") != keys.end());
    REQUIRE(std::find(vals.begin(), vals.end(), "val1") != vals.end());
    REQUIRE(std::find(vals.begin(), vals.end(), "val2") != vals.end());

    cleanup();
}

TEST_CASE("LettuceDatabase hlen returns correct number of fields", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.hset("myhash", "field1", "val1");
    db.hset("myhash", "field2", "val2");
    REQUIRE(db.hlen("myhash") == 2);

    cleanup();
}

TEST_CASE("LettuceDatabase hmset sets multiple fields", "[database]")
{
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    std::vector<std::pair<std::string, std::string>> pairs = {
        {"f1", "v1"}, {"f2", "v2"}, {"f3", "v3"}};
    REQUIRE(db.hmset("myhash", pairs));
    REQUIRE(db.hlen("myhash") == 3);
    std::string value;
    REQUIRE(db.hget("myhash", "f2", value));
    REQUIRE(value == "v2");

    cleanup();
}

TEST_CASE("LettuceDatabase purgeExpired removes expired keys", "[database]") {
    LettuceDatabase &db = LettuceDatabase::getInstance();
    db.flushAll();

    db.set("foo", "bar");
    db.expire("foo", 1); // expire in 1 second

    // Wait for key to expire
    std::this_thread::sleep_for(std::chrono::seconds(2));
    db.purgeExpired();

    std::string value;
    REQUIRE_FALSE(db.get("foo", value)); // Should be gone

    cleanup();
}