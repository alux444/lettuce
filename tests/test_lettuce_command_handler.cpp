#include <../external/catch2/catch.hpp>
#include "../include/LettuceCommandHandler.h"

TEST_CASE("parseRespCommand handles RESP arrays", "[resp]") {
    std::string input = "*2\r\n$4\r\nPING\r\n$4\r\nTEST\r\n";
    auto tokens = parseRespCommand(input);
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "PING");
    REQUIRE(tokens[1] == "TEST");
}

TEST_CASE("parseRespCommand handles inline commands", "[resp]") {
    std::string input = "PING TEST";
    auto tokens = parseRespCommand(input);
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "PING");
    REQUIRE(tokens[1] == "TEST");
}

TEST_CASE("LettuceCommandHandler returns PONG from PING request", "[handler]") {
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*1\r\n$4\r\nPING\r\n");
    REQUIRE(resp.find("+PONG") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler returns ERROR from UNKNOWN request", "[handler]") {
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*1\r\n$5\r\nHELLO\r\n");
    REQUIRE(resp.find("-ERR: Unknown command") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler returns arguments from ECHO request", "[handler]") {
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*2\r\n$4\r\nECHO\r\n$5\r\nwat\r\n");
    REQUIRE(resp.find("+wat") != std::string::npos);
}

TEST_CASE("LettuceCommandHandler enforces required argument for ECHO request", "[handler]") {
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*1\r\n$4\r\nECHO\r\n");
    REQUIRE(resp.find("-ERR: ECHO requires an argument") != std::string::npos);
}