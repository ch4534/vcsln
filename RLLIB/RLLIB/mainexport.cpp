#include "stdafx.h"
#include "PreDefine.h"
#include "exString.h"
#include "Wincap.h"
#include "FileFind.h"
#include <fstream>

/**
* @brief ��ȡ��ƷINI��Ϣ
*
* @param char*** ppStringBufferIni �����ַ������鷵�ؽ�Ʒ��Ϣ
* @param int* pStringCount ��������ĸ���
*/
DLLEXPORT void _stdcall LoadLotteryIniInfo(char*** ppStringBufferIni, int* pStringsCount)
{
	//��ȡini�ļ����н�Ʒ��Ϣ��¼��
	std::vector<std::string> listLottery;

	//CFileFind filefind;
	FileFind filefind;
	if (filefind.IsFileExist(L"set.ini"))
	{
		TCHAR strKeyNameTemp[2048];
		TCHAR strValueTemp[256];
		//filefind.FindNextFileW();
		std::wstring IniPath = filefind.getFilePath();
		DWORD dwKeyNameSize = GetPrivateProfileString(L"LotteryName", NULL, NULL, strKeyNameTemp, 2048, IniPath.c_str());
		//DWORD dw = GetLastError();
		if (dwKeyNameSize > 0)
		{
			TCHAR* pKeyName = new TCHAR[dwKeyNameSize];
			int nKeyNameLen = 0;
			for (int j = 0; j < dwKeyNameSize; j++)
			{
				pKeyName[nKeyNameLen++] = strKeyNameTemp[j];
				if (strKeyNameTemp[j] == L'')
				{
					if (GetPrivateProfileString(L"LotteryName", pKeyName, NULL, strValueTemp, 256, IniPath.c_str()) > 0)
					{
						listLottery.push_back(exString::WideCharToMultiChar(strValueTemp));
					}

					nKeyNameLen = 0;
				}
			}

			delete[] pKeyName;
		}
	}

	*pStringsCount = listLottery.size();

	size_t stSizeOfArray = sizeof(char*) * listLottery.size();

	*ppStringBufferIni = (char**)::CoTaskMemAlloc(stSizeOfArray);
	memset(*ppStringBufferIni, 0, stSizeOfArray);

	for (int i = 0, count = listLottery.size(); i < count; i++)
	{
		(*ppStringBufferIni)[i] = (char*)::CoTaskMemAlloc(listLottery[i].length() + 1);
		strcpy((*ppStringBufferIni)[i], listLottery[i].c_str());
	}
}

/**
* @brief ���潱ƷINI��Ϣ
*
* @param char** pStringBufferIni �����ַ�������
* @param int StringsCount ���������ά��
*/
DLLEXPORT void _stdcall SaveLotteryIniInfo(char** pStringBufferIni, int StringsCount)
{
	std::vector<wstring> wstringIni;

	for (int i = 0; i < StringsCount; i++)
	{
		wstringIni.push_back(exString::MultiCharToWideChar(exString::WideCharToMultiChar((wchar_t*)(*(pStringBufferIni + i)))));
	}

	if (wstringIni.size() > 0)
	{
		//CFile file;
		std::ofstream ofs(L"set.ini", std::ios::trunc | std::ios::out);
		ofs.close();
		//file.Open(_T("set.ini"), CFile::modeCreate);
		std::wstring iniPath = FileFind::RelativeToAbsolute(L"set.ini");

		::WritePrivateProfileString(L"LotteryName", NULL, NULL, iniPath.c_str());

		for (int i = 0; i < wstringIni.size(); i++)
		{
			::WritePrivateProfileString(L"LotteryName", std::to_wstring(i).c_str(), wstringIni[i].c_str(), iniPath.c_str());
		}
	}
}

/**
* @brief ��ʼ��DLL�ļ�
*
* @return ����true��ʾ��ʼ���ɹ�������false��ʾ��ʼ��ʧ��
*/
DLLEXPORT bool _stdcall InitDll()
{
	bool initComplete = false;

	do
	{
		Wincap* wincap = Wincap::getInstance();
		if (wincap->initCap() == false)
		{
			break;
		}

		initComplete = true;
	} while (false);

	return initComplete;
}

/**
* @brief �õ���ǰ�豸�������豸
*
* @param char*** ppStringBufferDevice ���ص�ǰ�豸��������Ϣ
* @param int* pStringCount  ���ط��ص��ַ��������ά��
*/
DLLEXPORT void _stdcall GetNetDevice(char*** ppStringBufferDevice, int* pStringsCount)
{
	Wincap* wincap = Wincap::getInstance();
	auto allDevice = wincap->getAllDevice();

	*pStringsCount = allDevice.size();

	size_t stSizeOfArray = sizeof(char*) * allDevice.size();

	*ppStringBufferDevice = (char**)::CoTaskMemAlloc(stSizeOfArray);
	memset(*ppStringBufferDevice, 0, stSizeOfArray);

	for (int i = 0, count = allDevice.size(); i < count; i++)
	{
		(*ppStringBufferDevice)[i] = (char*)::CoTaskMemAlloc(strlen(allDevice[i]->description) + 1);
		strcpy((*ppStringBufferDevice)[i], allDevice[i]->description);
	}
}

/**
* @brief ��ָ���������豸
*
* @param int indexDev ��Ҫ���豸������ֵ
* 
* @return ����true��ʾ�򿪳ɹ�������false��ʾ��ʧ��
*/
DLLEXPORT bool _stdcall OpenNetDevice(int indexDev)
{
	Wincap* wincap = Wincap::getInstance();

	return wincap->OpenDevice(indexDev, 0);
}

/**
* @brief �رմ򿪵������豸
*/
DLLEXPORT void _stdcall CloseNetDevice()
{
	Wincap* wincap = Wincap::getInstance();
	wincap->CloseDevice();
}

/**
* @brief �õ��������ݰ��еĳ齱��Ϣ
* ���ﶼ��ʹ��Unicode�ַ�����
*
* @param int   iType         1��ʾ��ǰ�齱������Ϊս�죬2��ʾ��ǰ�齱������ΪBվ
* @param char* strLevel      ���ط�˿�ȼ�
* @param char* strScriptId   ���ط�˿��ID
* @param char* strScriptUid  ���ط�˿��UID
* @param char* strContent    ���ط�˿�ĵ�Ļ
* @param char* strScriptName ���ط�˿������
*/
DLLEXPORT void _stdcall GetScriptMsg(int iType, char** strLevel, char** strScriptId, char** strScriptUid, char** strContent, char** strScriptName)
{
	Wincap* wincap = Wincap::getInstance();

	auto RevData = wincap->getRevDataClean();

	if (RevData.first)
	{
		std::wstring revStr = RevData.second;

		if (iType == 1)
		{
			if (revStr.find(L"fromid") != std::wstring::npos)
			{
				Json::Reader reader;
				Json::Value root;

				if (reader.parse(exString::WideCharToMultiChar(revStr), root))
				{
					/*std::wstring level = std::to_wstring(root["level"].asInt());
					memcpy(*strLevel, level.c_str(), (level.length() + 1) * sizeof(wchar_t));
					std::wstring scriptId = std::to_wstring(root["fromid"].asInt64());
					memcpy(*strScriptId, scriptId.c_str(), (scriptId.length() + 1) * sizeof(wchar_t));
					std::wstring scriptUid = std::to_wstring(root["fromuid"].asInt64());
					memcpy(*strScriptUid, scriptUid.c_str(), (scriptUid.length() + 1) * sizeof(wchar_t));
					std::wstring content = exString::MultiCharToWideChar(root["content"].asCString());
					memcpy(*strContent, content.c_str(), (content.length() + 1) * sizeof(wchar_t));
					std::wstring name = exString::MultiCharToWideChar(root["fromname"].asCString());
					memcpy(*strScriptName, name.c_str(), (name.length() + 1) * sizeof(wchar_t));*/
					std::string level = std::to_string(root["level"].asInt());
					strcpy(*strLevel, level.c_str());
					std::string scriptId = std::to_string(root["fromid"].asInt64());
					strcpy(*strScriptId, scriptId.c_str());
					std::string scriptUid = std::to_string(root["fromuid"].asInt64());
					strcpy(*strScriptUid, scriptUid.c_str());
					std::string content = root["content"].asCString();
					strcpy(*strContent, content.c_str());
					std::string name = root["fromname"].asCString();
					strcpy(*strScriptName, name.c_str());
				}
			}
		}
		else if (iType == 2)
		{
			if (revStr.find(L"info") != std::wstring::npos)
			{
				Json::Reader reader;
				Json::Value root;

				if (reader.parse(exString::WideCharToMultiChar(revStr), root))
				{
					const Json::Value arrObj = root["info"];
					std::wstring level = exString::MultiCharToWideChar(arrObj[3][0].asCString());
					memcpy(*strLevel, level.c_str(), sizeof(wchar_t) * (level.length() + 1));
					std::wstring uid = std::to_wstring(arrObj[2][0].asInt64());
					memcpy(*strScriptId, uid.c_str(), (uid.length() + 1) * sizeof(wchar_t));
					memcpy(*strScriptUid, uid.c_str(), (uid.length() + 1) * sizeof(wchar_t));
					std::wstring content = exString::MultiCharToWideChar(arrObj[1].asCString());
					memcpy(*strContent, content.c_str(), (content.length() + 1) * sizeof(wchar_t));
					std::wstring name = exString::MultiCharToWideChar(arrObj[2][1].asCString());
					memcpy(*strScriptName, name.c_str(), (name.length() + 1) * sizeof(wchar_t));
				}
			}
		}
	}
}