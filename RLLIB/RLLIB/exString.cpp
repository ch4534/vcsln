#include "stdafx.h"
#include "exString.h"

exString::exString()
{
}

exString::~exString()
{

}

/**
* @brief ���ַ�ת���ֽ��ַ�
*
* @param wstring wstr ������Ҫ����ת���Ŀ��ַ���
*
* @return ����ת����ɵĶ��ֽ��ַ���
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
* @brief ���ַ�ת���ֽ��ַ�
*
* @param wchar_t* wstr ������ַ�C����ַ���
*
* @return ����ת����ɵ��ַ���
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
* @brief ���ֽ��ַ���ת�����ֽ��ַ���
*
* @param string mstr ������ֽ��ַ���
*
* @return ����ת����ɵĿ��ֽ��ַ���
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
* @brief ���ֽ��ַ���ת���ַ��ַ���
*
* @param char* mstr
*
* @return ����ת����ɵĿ��ֽ��ַ���
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