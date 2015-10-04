#pragma once
#define _AFXDLL

#include <afx.h>
#include <iostream>
#include <vector>
#include <string>
#include <reader.h>
#include <value.h>
#include <writer.h>

#pragma comment(lib, "wpcap.lib")

#ifdef _DEBUG
#pragma comment(lib, "lib_jsond.lib")
#else
#pragma comment(lib, "lib_json.lib")
#endif