#pragma once
#include <string>

using std::string;
using std::wstring;

class exString : public CString
{
public:
	exString();
	exString(UINT uID);
	~exString();

	static string WideCharToMultiChar(wstring wstr);
	static string WideCharToMultiChar(wchar_t* wstr);
	static wstring MultiCharToWideChar(string mstr);
	static wstring MultiCharToWideChar(char* mstr);
};

