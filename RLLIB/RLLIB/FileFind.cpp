#include "stdafx.h"
#include "FileFind.h"
#include <fstream>
#include <regex>


FileFind::FileFind()
{
}


FileFind::~FileFind()
{
}

/**
* @brief �ж��ļ��Ƿ����
*
* @param std::wstring path ������Ҫ�жϵ��ļ�
*
* @return bool ����true��ʾ�ļ����ڣ�����false��ʾ�ļ�δ�ҵ�
*/
bool FileFind::IsFileExist(std::wstring path)
{
	if (this->IsAbsolutePath(path))
	{
		mAbsolutePath = path;
	}
	else
	{
		mAbsolutePath = RelativeToAbsolute(path);
	}

	std::ifstream fin(path);
	if (!fin)
	{
		return false;
	}
	else
	{
		fin.close();
		return true;
	}
}

/**
* @brief �ж��ļ����Ƿ����
*
* @param std::wstring path �ļ���·��
*
* @return ����true��ʾ�ļ��д��ڣ�����false��ʾ�ļ��в�����
*/
bool FileFind::IsDirectoryExist(std::wstring path)
{
	WIN32_FIND_DATA wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(path.c_str(), &wfd);
	if (hFind != INVALID_HANDLE_VALUE && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return true;
	}

	return false;
}

/**
* @brief �ж��Ƿ�Ϊ����·��
*
* @param std::wstring path ������Ҫ�жϵ�·��
*
* @return ����true��ʾΪ����·��������false��ʾ���Ǿ���·��
*/
bool FileFind::IsAbsolutePath(std::wstring path)
{
	std::wregex pattern(LR"(^(/|([[:alpha:]]:[\\/])).*)");
	std::wcmatch result;

	return std::regex_match(path.c_str(), result, pattern);
}

/**
* @brief �����·��ת��Ϊ����·��
*
* @param std::wstring path ������Ҫת�������·��
*
* @return ���ؾ���·��
*/
std::wstring FileFind::RelativeToAbsolute(std::wstring path)
{
	TCHAR fullPath[2048];
	ZeroMemory(fullPath, 2048);
	::GetFullPathName(path.c_str(), 2048, fullPath, NULL);

	return std::wstring(fullPath);
}

/**
* @brief �����ҵ��ļ��ľ���·��
*
* @return ���ؾ���·���ַ���
*/
std::wstring FileFind::getFilePath()
{
	return this->mAbsolutePath;
}
