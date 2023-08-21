#pragma once
#include <string>

#define		WX_VERSION		"3.1.0.41"



class HUtil {
public:
	HUtil();
	~HUtil();

	DWORD Offset(DWORD offset);

	void OpenConsole();
	void CloseConsole();

private:
	DWORD mBase = NULL;

	FILE* mConsoleOut;
	FILE* mConsolein;
	std::streambuf* mConsoleOutBackup;
	BOOL mConsoleOpen = FALSE;

public:
	static std::string GetWeChatVersion();
};