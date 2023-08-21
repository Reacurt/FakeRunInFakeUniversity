#include<string>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include"socketHTTP.h"
#include <ctime>
#include <chrono>
#include <thread>
#include <queue>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

queue<SOCKET> socket_list;
SOCKET temp;

string check_request(const char* requ) {
    //printf("%s\n", requ);
    string sbuf = requ;
    int first_space_pos = sbuf.find(" ");
    string method = sbuf.substr(0, first_space_pos);
    if (method != "GET") {
        return "flase";
    }
    int second_space_pos = sbuf.find(" ", first_space_pos + 1);
    string path = sbuf.substr(first_space_pos + 1, second_space_pos - first_space_pos - 1);
    if (path == "/getCode") {
        return "getCode";
    }
    if (path == "/sendCode") {
        int code_pos = sbuf.find("\n\n");
        string code = sbuf.substr(code_pos + 2, 32);
        return code;
    }
    return "flase";
}

HttpServer::HttpServer(const char* ip, int port) {
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("ws start false:%d\n", WSAGetLastError());
        exit(0);
    }

    srv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (srv_socket == INVALID_SOCKET) {
        printf("srv socket error: %d\n", WSAGetLastError());
        exit(0);
    }

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &srv_addr.sin_addr.S_un.S_addr);
    int is_bing = bind(srv_socket, (sockaddr*)&srv_addr, sizeof(srv_addr));
    if (is_bing == SOCKET_ERROR) {
        printf("bind error: %d\n", WSAGetLastError());
        exit(0);
    }
}


void HttpServer::start() {
    int is_listen = listen(srv_socket, SOMAXCONN);
    if (is_listen == SOCKET_ERROR) {
        printf("listen error: %d\n", WSAGetLastError());
        exit(0);
    }
    while (flag) {
        sockaddr_in cli_addr;
        int cli_addr_len = sizeof(cli_addr);

        SOCKET conn_socket = accept(srv_socket, (sockaddr*)&cli_addr, &cli_addr_len);
        if (conn_socket == INVALID_SOCKET) {
            printf("\naccept error : %d\n", WSAGetLastError());
            continue;
            exit(0);
        }

        int timeout = 2000;
        setsockopt(conn_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));

        time_t t = time(0);
        char now[0x50] = { 0 };
        ctime_s(now,0x50,&t);
        char cli_addr_buf[20] = { 0 };
        inet_ntop(AF_INET, &cli_addr.sin_addr.S_un.S_addr, cli_addr_buf, sizeof(cli_addr_buf));
        printf("%s accept from ip:%s\n", now, cli_addr_buf);

        char recv_buf[0x100] = { 0 };
        recv(conn_socket, recv_buf, sizeof(recv_buf), 0);
        
        string check_result = check_request(recv_buf);
        if (check_result=="false")
        {
            int is_send = send(conn_socket, bad_buf.c_str(), bad_buf.length(), 0);
            if (is_send == SOCKET_ERROR) {
                printf("send error: %d\n", WSAGetLastError());
                continue;
                exit(0);
            }
        }
        else
        {
            if (check_result=="getCode")
            {
                DWORD wx_pid = injectDll();
                this_thread::sleep_for(std::chrono::milliseconds(500));
                socket_list.push(conn_socket);
                enjectDll(wx_pid);
                continue;
            }
            else
            {
                if (socket_list.empty())
                {
                    continue;
                }
                temp = socket_list.front();
                socket_list.pop();
                printf("%s\n", check_result.c_str());
                string res = buf + check_result;
                int is_send = send(temp, res.c_str(), res.length(), 0);
                if (is_send == SOCKET_ERROR) {
                    printf("send error: %d\n", WSAGetLastError());
                    continue;
                    exit(0);
                }
                closesocket(temp);
            }
        }
        closesocket(conn_socket);
    }
    return;
}

void HttpServer::stop() {
    flag = false;
    closesocket(srv_socket);
    WSACleanup();
}

HttpServer::~HttpServer() {
    stop();
    closesocket(srv_socket);
    WSACleanup();
}


HttpClient::HttpClient(const char* ip, int port) {
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("ws start false:%d\n", WSAGetLastError());
        exit(0);
    }

    cli_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (cli_socket == INVALID_SOCKET) {
        printf("cli socket error: %d\n", WSAGetLastError());
        exit(0);
    }

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &cli_addr.sin_addr.S_un.S_addr);
    int connect_res = connect(cli_socket, (sockaddr*)&cli_addr, sizeof(cli_addr));
    if (connect_res == SOCKET_ERROR) {
        printf("cli connect error: %d\n", WSAGetLastError());
        exit(0);
    }

}

void HttpClient::send_msg() {
    //HookAndGetCode();
    //callLoginAPI();
    //this_thread::sleep_for(std::chrono::milliseconds(500));
    //string temp_code = getcode();
    //string res = buf + temp_code;
    //send(cli_socket, res.c_str(), res.length(), 0);
    //UnHookAndGetCode();
    return;
}

HttpClient::~HttpClient() {
    closesocket(cli_socket);
    WSACleanup();
}