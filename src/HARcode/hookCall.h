#pragma once
#include<string>
#include"HWeChat.h"

using namespace std;

void callLoginAPI();
//__declspec(naked) void HookCode();
void getCode(DWORD eax);
void HookAndGetCode();
void UnHookAndGetCode();
string getcode();