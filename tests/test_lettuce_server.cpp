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
    static LettuceServer server(port);
    test_server = &server;
    server.run();
}

std::string send_command(const std::string& host, int port, const std::string& command) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    REQUIRE(sock >= 0);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr);

    REQUIRE(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0);

    send(sock, command.c_str(), command.size(), 0);

    char buffer[1024] = {0};
    int valread = recv(sock, buffer, sizeof(buffer)-1, 0);
    close(sock);

    return std::string(buffer, valread);
}

TEST_CASE("LettuceServer responds to PING", "[integration]") {
    int port = 6389;
    std::thread server_thread(start_server, port);
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Give server time to start

    std::string resp = send_command("127.0.0.1", port, "*1\r\n$4\r\nPING\r\n");
    REQUIRE(resp.find("+OK") != std::string::npos);

    test_server->shutdown();
    server_thread.join();
}