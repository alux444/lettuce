#include <../external/catch2/catch.hpp>
#include "../include/LettuceCommandHandler.h"

#include <iostream>

TEST_CASE("parseRespCommand handles RESP arrays", "[resp]")
{
    std::string input = "*2\r\n$4\r\nPING\r\n$4\r\nTEST\r\n";
    auto tokens = parseRespCommand(input);
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "PING");
    REQUIRE(tokens[1] == "TEST");
}

TEST_CASE("parseRespCommand handles inline commands", "[resp]")
{
    std::string input = "PING TEST";
    auto tokens = parseRespCommand(input);
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "PING");
    REQUIRE(tokens[1] == "TEST");
}

TEST_CASE("LettuceCommandHandler returns PONG from PING request", "[handler]")
{
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*1\r\n$4\r\nPING\r\n");
    REQUIRE(resp.find("+PONG") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler returns ERROR from UNKNOWN request", "[handler]")
{
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*1\r\n$5\r\nHELLO\r\n");
    REQUIRE(resp.find("-ERR: Unknown command") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler returns arguments from ECHO request", "[handler]")
{
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*2\r\n$4\r\nECHO\r\n$5\r\nwat\r\n");
    REQUIRE(resp.find("+wat") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler enforces required argument for ECHO request", "[handler]")
{
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*1\r\n$4\r\nECHO\r\n");
    REQUIRE(resp.find("-ERR: ECHO requires an argument") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler LGET returns all list elements", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$6\r\naalist\r\n$1\r\nx\r\n");
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$6\r\naalist\r\n$1\r\ny\r\n");
    handler.handleCommand("*3\r\n$5\r\nLPUSH\r\n$6\r\naalist\r\n$1\r\nz\r\n");
    std::string lget_resp = handler.handleCommand("*2\r\n$4\r\nLGET\r\n$6\r\naalist\r\n");
    // Should be z, x, y
    REQUIRE(lget_resp.find("$1\r\nz\r\n") != std::string::npos);
    REQUIRE(lget_resp.find("$1\r\nx\r\n") != std::string::npos);
    REQUIRE(lget_resp.find("$1\r\ny\r\n") != std::string::npos);
    // Should have 3 elements in RESP array
    REQUIRE(lget_resp.find("*3\r\n") == 0);
}

TEST_CASE("LettuceCommandHandler LGET on empty or missing list", "[handler]")
{
    LettuceCommandHandler handler;
    std::string lget_resp = handler.handleCommand("*2\r\n$4\r\nLGET\r\n$6\r\nnope\r\n");
    // Should be an empty RESP array
    REQUIRE(lget_resp == "*0\r\n");
}

TEST_CASE("LettuceCommandHandler LLEN returns correct length", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*3\r\n$5\r\nLPUSH\r\n$6\r\nmylist\r\n$1\r\na\r\n");
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$6\r\nmylist\r\n$1\r\nb\r\n");
    std::string resp = handler.handleCommand("*2\r\n$4\r\nLLEN\r\n$6\r\nmylist\r\n");
    REQUIRE(resp.find(":2") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler LPOP and RPOP remove elements", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*3\r\n$5\r\nLPUSH\r\n$6\r\nmylist\r\n$1\r\nx\r\n");
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$6\r\nmylist\r\n$1\r\ny\r\n");
    std::string lpop_resp = handler.handleCommand("*2\r\n$4\r\nLPOP\r\n$6\r\nmylist\r\n");
    REQUIRE(lpop_resp.find("$1\r\nx\r\n") != std::string::npos);
    std::string rpop_resp = handler.handleCommand("*2\r\n$4\r\nRPOP\r\n$6\r\nmylist\r\n");
    REQUIRE(rpop_resp.find("$1\r\ny\r\n") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler LREM removes elements", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$6\r\nmylist\r\n$1\r\nf\r\n");
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$6\r\nmylist\r\n$1\r\nf\r\n");
    std::string lrem_resp = handler.handleCommand("*4\r\n$4\r\nLREM\r\n$6\r\nmylist\r\n$1\r\n0\r\n$1\r\nf\r\n");
    REQUIRE(lrem_resp.find(":2") != std::string::npos); // 2 elements removed
}

TEST_CASE("LettuceCommandHandler LINDEX gets element at index", "[handler]")
{
    LettuceCommandHandler handler;
    std::string rpush_resp = handler.handleCommand("*3\r\n$5\r\nLPUSH\r\n$6\r\nmylist\r\n$1\r\nz\r\n");
    std::cout << "RESPOSNE RPUSH" << rpush_resp;
    std::string lindex_resp = handler.handleCommand("*3\r\n$6\r\nLINDEX\r\n$6\r\nmylist\r\n$1\r\n0\r\n");
    std::cout << "RESPOSNE LINDEX" << lindex_resp;
    REQUIRE(lindex_resp.find("$1\r\nz\r\n") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler LSET sets element at index", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$6\r\nmylist\r\n$1\r\na\r\n");
    std::string lset_resp = handler.handleCommand("*4\r\n$4\r\nLSET\r\n$6\r\nmylist\r\n$1\r\n0\r\n$1\r\nf\r\n");
    REQUIRE(lset_resp.find("+OK") != std::string::npos);
    std::string lindex_resp = handler.handleCommand("*3\r\n$6\r\nLINDEX\r\n$6\r\nmylist\r\n$1\r\n0\r\n");
    REQUIRE(lindex_resp.find("$1\r\nf\r\n") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler LPUSH adds elements to the left", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*3\r\n$5\r\nLPUSH\r\n$6\r\nmylist\r\n$1\r\na\r\n");
    handler.handleCommand("*3\r\n$5\r\nLPUSH\r\n$6\r\nmylist\r\n$1\r\nb\r\n");
    std::string lindex0 = handler.handleCommand("*3\r\n$6\r\nLINDEX\r\n$6\r\nmylist\r\n$1\r\n0\r\n");
    std::string lindex1 = handler.handleCommand("*3\r\n$6\r\nLINDEX\r\n$6\r\nmylist\r\n$1\r\n1\r\n");
    REQUIRE(lindex0.find("$1\r\nb\r\n") != std::string::npos); // b is now at the head
    REQUIRE(lindex1.find("$1\r\na\r\n") != std::string::npos); // a is now at index 1
}

TEST_CASE("LettuceCommandHandler RPUSH adds elements to the right", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$5\r\nlisty\r\n$1\r\nn\r\n");
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$5\r\nlisty\r\n$1\r\nj\r\n");
    std::string lindex0 = handler.handleCommand("*3\r\n$6\r\nLINDEX\r\n$5\r\nlisty\r\n$1\r\n0\r\n");
    std::string lindex1 = handler.handleCommand("*3\r\n$6\r\nLINDEX\r\n$5\r\nlisty\r\n$1\r\n1\r\n");
    REQUIRE(lindex0.find("$1\r\nn\r\n") != std::string::npos);
    REQUIRE(lindex1.find("$1\r\nj\r\n") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler HSET and HGET", "[handler]")
{
    LettuceCommandHandler handler;
    std::string hset_resp = handler.handleCommand("*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$5\r\nfield\r\n$5\r\nvalue\r\n");
    REQUIRE(hset_resp.find(":1") != std::string::npos);
    std::string hget_resp = handler.handleCommand("*3\r\n$4\r\nHGET\r\n$6\r\nmyhash\r\n$5\r\nfield\r\n");
    REQUIRE(hget_resp.find("$5\r\nvalue\r\n") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler HEXISTS and HDEL", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$3\r\nfoo\r\n$3\r\nbar\r\n");
    std::string hexists_resp = handler.handleCommand("*3\r\n$7\r\nHEXISTS\r\n$6\r\nmyhash\r\n$3\r\nfoo\r\n");
    REQUIRE(hexists_resp.find(":1") != std::string::npos);
    std::string hdel_resp = handler.handleCommand("*3\r\n$4\r\nHDEL\r\n$6\r\nmyhash\r\n$3\r\nfoo\r\n");
    REQUIRE(hdel_resp.find(":1") != std::string::npos);
    std::string hexists2_resp = handler.handleCommand("*3\r\n$7\r\nHEXISTS\r\n$6\r\nmyhash\r\n$3\r\nfoo\r\n");
    REQUIRE(hexists2_resp.find(":0") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler HGETALL returns all fields and values", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$2\r\nf1\r\n$2\r\nv1\r\n");
    handler.handleCommand("*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$2\r\nf2\r\n$2\r\nv2\r\n");
    std::string hgetall_resp = handler.handleCommand("*2\r\n$7\r\nHGETALL\r\n$6\r\nmyhash\r\n");
    REQUIRE(hgetall_resp.find("$2\r\nf1\r\n$2\r\nv1\r\n") != std::string::npos);
    REQUIRE(hgetall_resp.find("$2\r\nf2\r\n$2\r\nv2\r\n") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler HKEYS and HVALS", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$2\r\nk1\r\n$2\r\nv1\r\n");
    handler.handleCommand("*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$2\r\nk2\r\n$2\r\nv2\r\n");
    std::string hkeys_resp = handler.handleCommand("*2\r\n$5\r\nHKEYS\r\n$6\r\nmyhash\r\n");
    REQUIRE(hkeys_resp.find("$2\r\nk1\r\n") != std::string::npos);
    REQUIRE(hkeys_resp.find("$2\r\nk2\r\n") != std::string::npos);
    std::string hvals_resp = handler.handleCommand("*2\r\n$5\r\nHVALS\r\n$6\r\nmyhash\r\n");
    REQUIRE(hvals_resp.find("$2\r\nv1\r\n") != std::string::npos);
    REQUIRE(hvals_resp.find("$2\r\nv2\r\n") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler HLEN returns correct number of fields", "[handler]")
{
    LettuceCommandHandler handler;
    handler.handleCommand("*4\r\n$4\r\nHSET\r\n$6\r\nssssss\r\n$2\r\nf1\r\n$2\r\nv1\r\n");
    handler.handleCommand("*4\r\n$4\r\nHSET\r\n$6\r\nssssss\r\n$2\r\nf2\r\n$2\r\nv2\r\n");
    std::string hlen_resp = handler.handleCommand("*2\r\n$4\r\nHLEN\r\n$6\r\nssssss\r\n");
    REQUIRE(hlen_resp.find(":2") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler HMSET sets multiple fields", "[handler]")
{
    LettuceCommandHandler handler;
    std::string hmset_cmd = "*6\r\n$5\r\nHMSET\r\n$6\r\nmyhash\r\n$2\r\nf1\r\n$2\r\nv1\r\n$2\r\nf2\r\n$2\r\nv2\r\n";
    std::string hmset_resp = handler.handleCommand(hmset_cmd);
    REQUIRE(hmset_resp.find("+OK") != std::string::npos);
    std::string hget_resp = handler.handleCommand("*3\r\n$4\r\nHGET\r\n$6\r\nmyhash\r\n$2\r\nf2\r\n");
    REQUIRE(hget_resp.find("$2\r\nv2\r\n") != std::string::npos);
}