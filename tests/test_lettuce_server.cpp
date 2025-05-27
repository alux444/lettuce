#include <catch2/catch.hpp>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/LettuceServer.h"
#include <signal.h>

LettuceServer* test_server = nullptr;

void start_server(int port) {
    LettuceServer* server = new LettuceServer(port);
    test_server = server;
    server->run();
}

void shutdown_server(std::thread& server_thread) {
    test_server->shutdown();
    server_thread.join();
    delete test_server;
    test_server = nullptr;
}

std::string send_command(const std::string& host, int port, const std::string& command) {
    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    REQUIRE(sock >= 0);

    // set up server address, family, port
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr); // convert presentation (readable ip) to binary usable by network

    // connect to the server
    REQUIRE(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0);

    send(sock, command.c_str(), command.size(), 0);

    char buffer[1024] = {0};
    int valread = recv(sock, buffer, sizeof(buffer)-1, 0);
    close(sock);

    return std::string(buffer, valread);
}

TEST_CASE("LettuceServer responds to PING with PONG", "[integration]") {
    int port = 6389;
    std::thread server_thread(start_server, port);
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Give server time to start

    std::string resp = send_command("127.0.0.1", port, "*1\r\n$4\r\nPING\r\n");
    REQUIRE(resp.find("+PONG") != std::string::npos);

    shutdown_server(server_thread);
}

TEST_CASE("LettuceServer responds with -ERR with Unknown Command", "[integration]") {
    int port = 6389;
    std::thread server_thread(start_server, port);
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Give server time to start

    std::string resp = send_command("127.0.0.1", port, "*1\r\n$4\r\nHELLO\r\n");
    REQUIRE(resp.find("-ERR") != std::string::npos);

    shutdown_server(server_thread);
}