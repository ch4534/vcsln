#include "stdafx.h"
#include "PreDefine.h"
#include "exString.h"
#include "Wincap.h"
#include <vector>
#include <string>

DLLEXPORT void _stdcall LoadLotteryIniInfo(char*** ppStringBufferIni, int* pStringsCount)
{
	//读取ini文件进行奖品信息的录入
	std::vector<std::string> listLottery;

	CFileFind filefind;
	if (filefind.FindFile(_T("set.ini")))
	{
		TCHAR strKeyNameTemp[2048];
		TCHAR strValueTemp[256];
		filefind.FindNextFileW();
		CString IniPath = filefind.GetFilePath();
		DWORD dwKeyNameSize = GetPrivateProfileString(L"LotteryName", NULL, NULL, strKeyNameTemp, 2048, IniPath);
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
					if (GetPrivateProfileString(L"LotteryName", pKeyName, NULL, strValueTemp, 256, IniPath) > 0)
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

DLLEXPORT void _stdcall SaveLotteryIniInfo(char** pStringBufferIni, int StringsCount)
{
	std::vector<wstring> wstringIni;

	for (int i = 0; i < StringsCount; i++)
	{
		wstringIni.push_back(exString::MultiCharToWideChar(exString::WideCharToMultiChar((wchar_t*)(*(pStringBufferIni + i)))));
	}

	if (wstringIni.size() > 0)
	{
		CFile file;
		file.Open(_T("set.ini"), CFile::modeCreate);
		CString iniPath = file.GetFilePath();
		file.Close();

		::WritePrivateProfileString(L"LotteryName", NULL, NULL, iniPath);

		for (int i = 0; i < wstringIni.size(); i++)
		{
			::WritePrivateProfileString(L"LotteryName", std::to_wstring(i).c_str(), wstringIni[i].c_str(), iniPath);
		}
	}
}

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