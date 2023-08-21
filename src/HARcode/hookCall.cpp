#include "pch.h"
#include "HWeChat.h"
#include <iostream>
#include <string.h>
#include"hookCall.h"

using namespace std;

HUtil* Util = new HUtil();
DWORD codeCallAddress = Util->Offset(0x6AD87E60 - 0x6A630000);
DWORD codeHookAddress = Util->Offset(0x6B608071 - 0x6A630000);
DWORD loginCallAddress = Util->Offset(0x6BAD7A30 - 0x6AB10000);
DWORD codeReturnAddress = codeHookAddress + 5;
CHAR codeHookBackUp[5] = { 0 };
string tempCode = "";


__declspec(naked) void HookCode() {
    __asm {
        pushad;
        pushfd;

        push eax;
        call getCode;
        add esp, 0x4;

        popfd;
        popad;
        call codeCallAddress;
        jmp codeReturnAddress;
    }
}

void getCode(DWORD eax) {
    if (eax == 0) {
        return;
    }
    string code = HTools::ReadAsciiString(eax);
    cout << "code: " << code.c_str() << endl;
    tempCode = code;
}

void HookAndGetCode() {
    BYTE JMPCODE[5] = { 0 };
    JMPCODE[0] = 0xE9;
    *(DWORD*)&JMPCODE[1] = (DWORD)HookCode - codeHookAddress - 5;
    ReadProcessMemory(GetCurrentProcess(), (LPVOID)codeHookAddress, (LPVOID)codeHookBackUp, 5, 0);
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)codeHookAddress, (LPVOID)JMPCODE, 5, 0);
}

void UnHookAndGetCode() {
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)codeHookAddress, (LPVOID)codeHookBackUp, 5, 0);
}

void callLoginAPI() {
    DWORD a = 0x99;//网络请求的id，不影响可以任意值
    DWORD b = 0x21;
    CHAR requ[] = "{\"login_type\":1,\"version_type\":1}";
    CHAR login[] = "login";
    CHAR appid[] = "wx1680cca9a19ee3c8";
    /*CHAR appid[] = "wxf2f48f5d42e7f05e";*/
    __asm {
        pushad;
        pushfd;

        push a;
        push b;
        lea eax, requ;
        push eax;
        lea eax, login;
        push eax;
        lea eax, appid;
        push eax;

        lea edx, appid;

        call loginCallAddress;
        add esp, 0x14;

        popfd;
        popad;
    }

}

string getcode() {
    return tempCode;
}