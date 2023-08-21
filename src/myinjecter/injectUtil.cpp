#include <Windows.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include <string>
#include <TlHelp32.h>
#include <thread>
#include <chrono>
#include"injectUtil.h"


#define DLL_NAME "HARcode.dll"
#define PROCESS_NAME "WeChat.exe"
#define MAX_SIZE 0x250
using namespace std;


bool isHcodeDllExists() {
    ifstream find(DLL_NAME);
    if (!find) {
        return false;
    }
    return true;
}

void getFullPath(char* buff) {
    char pwd[MAX_SIZE];
    _getcwd(pwd, MAX_SIZE);
    strcat_s(pwd, "\\");
    strcat_s(pwd, DLL_NAME);
    strcpy_s(buff, MAX_SIZE, pwd);
    //return pwd;
}

DWORD getProcessPidByName() {
    PROCESSENTRY32 process_info = { 0 };
    process_info.dwSize = sizeof(process_info);
    HANDLE process_snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(process_snap_shot, &process_info)) {
        do {
            if (strcmp(process_info.szExeFile, PROCESS_NAME) == 0) {
                CloseHandle(process_snap_shot);
                return process_info.th32ProcessID;
            }
        } while (Process32Next(process_snap_shot, &process_info));
    }
    CloseHandle(process_snap_shot);
    return 0;
}

void haveError(char const* error_msg) {
    printf("%s", error_msg);
    system("pause");
    exit(0);
}

DWORD injectDll() {
    if (!isHcodeDllExists())
    {
        haveError("当前目录下未找到 Hcode.dll");
    }
    char full_path[MAX_SIZE];
    getFullPath(full_path);
    DWORD wx_pid = getProcessPidByName();
    if (wx_pid == 0) {
        haveError("微信未运行");
    }
    HANDLE wx_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, wx_pid);
    if (NULL == wx_process)
    {
        haveError("打开微信失败");
    }

    LPVOID remote_mem_addr = VirtualAllocEx(wx_process, NULL, MAX_SIZE, MEM_COMMIT, PAGE_READWRITE);
    if (NULL == remote_mem_addr) {
        haveError("申请内存失败");
    }
    if (!WriteProcessMemory(wx_process, remote_mem_addr, full_path, MAX_SIZE, NULL)) {
        haveError("写入地址失败");
    }
    HANDLE remote_thread = CreateRemoteThread(wx_process, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, remote_mem_addr, 0, NULL);
    if (!remote_thread)
    {
        haveError("创建进程失败");
    }

    return wx_pid;
}

void enjectDll(DWORD wx_pid) {
    BOOL find = FALSE;
    MODULEENTRY32 moudle_info = { 0 };
    moudle_info.dwSize = sizeof(moudle_info);
    HANDLE moudle_snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, wx_pid);
    if (Module32First(moudle_snap_shot, &moudle_info)) {
        do {
            if (strcmp(moudle_info.szModule, DLL_NAME) == 0)
            {
                find = TRUE;
                break;
            }
        } while (Module32Next(moudle_snap_shot, &moudle_info));
    }
    if (!find)
    {
        haveError("未找到模块");
    }
    HANDLE wx_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, wx_pid);
    if (!wx_process) {
        haveError("微信打开失败");
    }
    HANDLE remote_thread = CreateRemoteThread(wx_process, NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, moudle_info.modBaseAddr, 0, NULL);
    if (!remote_thread)
    {
        haveError("卸载失败");
    }
}