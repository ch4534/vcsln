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
* @brief 判断文件是否存在
*
* @param std::wstring path 传入需要判断的文件
*
* @return bool 返回true表示文件存在，返回false表示文件未找到
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
* @brief 判断文件夹是否存在
*
* @param std::wstring path 文件夹路径
*
* @return 返回true表示文件夹存在，返回false表示文件夹不存在
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
* @brief 判断是否为绝对路径
*
* @param std::wstring path 传入需要判断的路径
*
* @return 返回true表示为绝对路径，返回false表示不是绝对路径
*/
bool FileFind::IsAbsolutePath(std::wstring path)
{
	std::wregex pattern(LR"(^(/|([[:alpha:]]:[\\/])).*)");
	std::wcmatch result;

	return std::regex_match(path.c_str(), result, pattern);
}

/**
* @brief 将相对路径转换为绝对路径
*
* @param std::wstring path 传入需要转换的相对路径
*
* @return 返回绝对路径
*/
std::wstring FileFind::RelativeToAbsolute(std::wstring path)
{
	TCHAR fullPath[2048];
	ZeroMemory(fullPath, 2048);
	::GetFullPathName(path.c_str(), 2048, fullPath, NULL);

	return std::wstring(fullPath);
}

/**
* @brief 返回找到文件的绝对路径
*
* @return 返回绝对路径字符串
*/
std::wstring FileFind::getFilePath()
{
	return this->mAbsolutePath;
}
