#pragma once

/**
* @brief 文件寻找类
*/
class FileFind
{
public:
	FileFind();
	virtual ~FileFind();

protected:
	std::wstring mAbsolutePath;//保存寻找到文件的绝对路径

protected:
	bool IsAbsolutePath(std::wstring path);

public:
	bool IsFileExist(std::wstring path);
	bool IsDirectoryExist(std::wstring path);
	std::wstring getFilePath();
	static std::wstring RelativeToAbsolute(std::wstring path);
};

