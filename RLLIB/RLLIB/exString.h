#pragma once
#include <string>

using std::string;
using std::wstring;

/**
* @brief ��չ�ַ�����
* @Author �̻�
* @Date 20150925
*/
class exString : public std::wstring
{
public:
	exString();
	~exString();

	static string WideCharToMultiChar(wstring wstr);
	static string WideCharToMultiChar(wchar_t* wstr);
	static wstring MultiCharToWideChar(string mstr);
	static wstring MultiCharToWideChar(char* mstr);
};

