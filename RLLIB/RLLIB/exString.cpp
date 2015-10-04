#include "stdafx.h"
#include "exString.h"

exString::exString()
{
}

exString::~exString()
{

}

/**
* @brief 宽字符转多字节字符
*
* @param wstring wstr 传入需要进行转换的宽字符串
*
* @return 返回转换完成的多字节字符串
*/
string exString::WideCharToMultiChar(wstring wstr)
{
	int MLen = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	char* MultiBte = new char[MLen + 1];

	int RetLen = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, MultiBte, MLen, NULL, NULL);

	MultiBte[MLen] = '\0';

	string strMuli = MultiBte;

	delete[] MultiBte;

	return strMuli;
}

/**
* @brief 宽字符转多字节字符
*
* @param wchar_t* wstr 传入宽字符C风格字符串
*
* @return 返回转换完成的字符串
*/
string exString::WideCharToMultiChar(wchar_t* wstr)
{
	int MLen = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);

	char* MultiBte = new char[MLen + 1];

	int RetLen = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, MultiBte, MLen, NULL, NULL);

	MultiBte[MLen] = '\0';

	string strMuli = MultiBte;

	delete[] MultiBte;

	return strMuli;
}

/**
* @brief 多字节字符串转换宽字节字符串
*
* @param string mstr 传入多字节字符串
*
* @return 返回转换完成的宽字节字符串
*/
wstring exString::MultiCharToWideChar(string mstr)
{
	int WLen = ::MultiByteToWideChar(CP_ACP, 0, mstr.c_str(), -1, NULL, 0);

	wchar_t* WideBte = new wchar_t[WLen + 1];

	int RetLen = ::MultiByteToWideChar(CP_ACP, 0, mstr.c_str(), -1, WideBte, WLen);

	WideBte[WLen] = '\0';

	wstring strMuli = WideBte;

	delete[] WideBte;

	return strMuli;
}

/**
* @brief 多字节字符串转宽字符字符串
*
* @param char* mstr
*
* @return 返回转换完成的宽字节字符串
*/
wstring exString::MultiCharToWideChar(char* mstr)
{
	int WLen = ::MultiByteToWideChar(CP_ACP, 0, mstr, -1, NULL, 0);

	wchar_t* WideBte = new wchar_t[WLen + 1];

	int RetLen = ::MultiByteToWideChar(CP_ACP, 0, mstr, -1, WideBte, WLen);

	WideBte[WLen] = '\0';

	wstring strMuli = WideBte;

	delete[] WideBte;

	return strMuli;
}