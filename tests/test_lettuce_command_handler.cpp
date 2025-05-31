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
    handler.handleCommand("*3\r\n$5\r\nRPUSH\r\n$6\r\nmylist\r\n$1\r\nz\r\n");
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