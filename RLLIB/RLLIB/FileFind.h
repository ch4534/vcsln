#pragma once

/**
* @brief �ļ�Ѱ����
*/
class FileFind
{
public:
	FileFind();
	virtual ~FileFind();

protected:
	std::wstring mAbsolutePath;//����Ѱ�ҵ��ļ��ľ���·��

protected:
	bool IsAbsolutePath(std::wstring path);

public:
	bool IsFileExist(std::wstring path);
	bool IsDirectoryExist(std::wstring path);
	std::wstring getFilePath();
	static std::wstring RelativeToAbsolute(std::wstring path);
};

