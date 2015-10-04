#pragma once
#include <array>
#include <vector>
#include <pcap.h>
#include "Protocol.h"

/**
* @brief 设计一个截取网络包的类
*/
class Wincap
{
protected:
	bool mbSaveFile          = false;
	int mDevCount            = 0;           //存储扫描到多少个设备
	pcap_t*    mAdhandle     = nullptr;     //存储当前打开的网络设备
	pcap_if_t* mAlldev       = nullptr;     //保存所有扫描到的设备索引
	pcap_dumper_t* mDumpfile = nullptr;     //保存引用保存到本地的文件索引
	pktcount mNpacket;                      //各类数据包的统计
	CPtrList mlocalDataList;				//保存被本地化后的数据包
	CPtrList mnetDataList;			    //保存从网络中直接获取的数据包
	char mErrbuf[PCAP_ERRBUF_SIZE];         //保存错误信息
	std::array<std::string, 5> mFilter;     //保存过滤器
	char mFilepath[512];					//文件保存路径
	char mFilename[64];						//文件名称		

protected:
	Wincap();

public:
	~Wincap();

public:
	//初始化设备
	bool initCap(bool bSave = false);

	//释放设备
	void freeCap();

	//得到所有的设备
	std::vector<pcap_if_t*> getAllDevice();

	//打开制定的设备
	bool OpenDevice(int selIndex, int selFilter = -1);

	//关闭打开的设备
	void CloseDevice();

	//得到执行错误的信息
	std::wstring getErrMsg();

	//得到过滤器
	std::vector<std::wstring> getFilter();

	//得到接收到的数据
	std::pair<bool, CString> getRevData();

	//得到接受释放内存的数据
	std::pair<bool, std::wstring> getRevDataClean();

	//得到捉包的实例对象
	static Wincap* getInstance();
};

