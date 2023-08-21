#pragma once
#include<string>
#include<WinSock2.h>
#include<WS2tcpip.h>

using namespace std;

bool check_request(const char* requ);

class HttpServer {
public:
    HttpServer(const char* ip, int port);
    ~HttpServer();
    void stop();
    void start();

private:
    string buf = "HTTP/1.1 200 OK\n"
        "Content-Length: 32\n"
        "Content-Type: text/plain\n"
        "\n";

    string bad_buf = "HTTP/1.1 400 OK\n"
        "Content-Length: 11\n"
        "Content-Type: text/plain\n"
        "\n"
        "Bad Request";
    WSADATA wsa_data;
    SOCKET srv_socket;
    sockaddr_in srv_addr;
    BOOL flag = true;
};

class HttpClient {
public:
    HttpClient(const char* ip, int port);
    ~HttpClient();
    void send_msg();

private:
    WSADATA wsa_data;
    SOCKET cli_socket;
    sockaddr_in cli_addr;
    string buf = "GET /sendCode HTTP / 1.1\n"
        "\n";
};