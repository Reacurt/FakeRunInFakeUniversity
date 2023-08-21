#pragma once
bool isHcodeDllExists();
void getFullPath(char* buff);
DWORD getProcessPidByName();
void haveError(char const* error_msg);
DWORD injectDll();
void enjectDll(DWORD wx_pid);