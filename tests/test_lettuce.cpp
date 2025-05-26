#define CATCH_CONFIG_MAIN
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

TEST_CASE("LettuceCommandHandler returns OK", "[handler]") {
    LettuceCommandHandler handler;
    std::string resp = handler.handleCommand("*1\r\n$4\r\nPING\r\n");
    REQUIRE(resp.find("+OK") != std::string::npos);
}