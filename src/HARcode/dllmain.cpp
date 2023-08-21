#include "pch.h"
#include "HWeChat.h"
#include <iostream>
#include <thread>
#include"httpserver.h"
#include"hookCall.h"
#include<ctime>
#include<fstream>
using namespace std;

HUtil* Utils = new HUtil();
HANDLE GHandle = NULL;


#define PORT_FILE "port.ini"
#define DLL_NAME "HARcode.dll"

string get_dll_path()
{
    HMODULE hModule = GetModuleHandleA(DLL_NAME);
    if (hModule) {
        char dllPath[MAX_PATH];
        GetModuleFileNameA(hModule, dllPath, MAX_PATH);
        return dllPath;
    }
    return "";
}


int get_port() {
	int port = 37020;
    string port_path = get_dll_path();
    port_path = port_path.substr(0, port_path.length() - 11) + PORT_FILE;
	ifstream ifs(port_path);
	if (ifs.good())
	{
		ifs >> port;
		ifs.close();
		return port;
	}
	else
	{
		ofstream ofs(port_path);
		ofs << port;
		ifs.close();
		ofs.close();
		return port;
	}
}


BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:{
			int port = get_port();            
            HttpClient new_cli("127.0.0.1", port);
            new_cli.send_msg();
            break;
        }
        case DLL_PROCESS_DETACH:{
            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}