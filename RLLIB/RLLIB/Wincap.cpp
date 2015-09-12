#include "stdafx.h"
#include "exString.h"
#include "Wincap.h"
#include "Utilities.h"

Wincap::Wincap()
{
	//初始化错误信息缓存
	ZeroMemory(mErrbuf, PCAP_ERRBUF_SIZE);

	//初始化过滤器
	mFilter[0] = "tcp";
	mFilter[1] = "udp";
	mFilter[2] = "ip";
	mFilter[3] = "icmp";
	mFilter[4] = "arp";
}


Wincap::~Wincap()
{

}

/**
* @brief 得到捉包实例
*
* @return 返回捉包实例指针
*/
Wincap* Wincap::getInstance(){
	static Wincap mSelf;

	return &mSelf;
}

/**
* @brief 初始化设备
*
* @return 返回true表示初始化成功，返回false表示初始化失败，使用函数得到错误信息
*/
bool Wincap::initCap()
{
	//如果已经找到设备，那么线释放
	if (mDevCount > 0)
	{
		freeCap();
	}

	//调用pcap_findalldevs扫描所有网络设备
	if (pcap_findalldevs(&mAlldev, mErrbuf) == -1)
	{
		return false;
	}

	//循环扫描每一个设备，计算所有的设备数量
	for (pcap_if_t* dev = mAlldev; dev != nullptr; dev = dev->next)
	{
		mDevCount++;
	}

	return true;
}

/**
* @brief 释放所有扫描到的设备
*/
void Wincap::freeCap()
{
	//如果设备总量大于零，表示扫描到设备，那么调用pcap_freealldevs释放设备
	if (mDevCount > 0)
	{
		pcap_freealldevs(mAlldev);
		mDevCount = 0;
	}
}

/**
* @brief 返回所有扫描到的设备
*
* @return std::vector<pcap_if_t*> 返回所有扫描到设备的数组
*/
std::vector<pcap_if_t*> Wincap::getAllDevice()
{
	std::vector<pcap_if_t*> devBuff;

	//将所有设备扫描一遍，存储在缓存中返回
	for (pcap_if_t* dev = mAlldev; dev != nullptr; dev = dev->next)
	{
		devBuff.push_back(dev);
	}

	return devBuff;
}

/**
* @brief 返回错误信息
*/
CString Wincap::getErrMsg()
{
	return CString(exString::MultiCharToWideChar(mErrbuf).c_str());
}

/**
* @brief 打开制定索引的设备
*
* @param int selIndex 传入需要打开设备的索引，索引号码从0开始
* @param int selFilter 传入包过滤器，默认值为-1，表示截取所有类型的包
*
* @return bool 返回true表示打开成功，返回false表示打开失败
*/
bool Wincap::OpenDevice(int selIndex, int selFilter/* = -1*/)
{
	bool bRet = false;
	u_int netmask;
	struct bpf_program fcode;

	do{
		//判断所有索引是否符合，如果不符合则跳出
		if ((selIndex < 0 || selIndex >= mDevCount) ||
			(selFilter != -1 && (selFilter < 0 || selFilter >= mFilter.size())))
		{
			const char* errormsg = "打开索引错误，请确认后再尝试";
			strcpy(mErrbuf, errormsg);
			break;
		}

		pcap_if_t* seldev = nullptr;
		pcap_if_t* dev = mAlldev;

		//根据索引找到需要打开的设备
		for (int i = 0; i < selIndex; i++)
		{
			dev = dev->next;
		}

		seldev = dev;

		//打开需要打开的设备
		if ((mAdhandle = pcap_open_live(seldev->name, 65536, 1, 1000, mErrbuf)) == nullptr)
		{
			const char* errormsg = "打开指定的设备出错，请确认后再尝试";
			strcpy(mErrbuf, errormsg);
			break;
		}

		//打开数据包连接
		if (pcap_datalink(mAdhandle) != DLT_EN10MB)
		{
			const char* errormsg = "这不适合于非以太网的网络";
			strcpy(mErrbuf, errormsg);
			break;
		}

		//如果打开设备的地址存在，那么掩码否则传入默认值
		if (dev->addresses != NULL)
			netmask = ((struct sockaddr_in *)(dev->addresses->netmask))->sin_addr.S_un.S_addr;
		else
			netmask = 0xffffff;

		//如果传入的过滤器是-1，那么表示所有类型的包都需要抓取
		if (selFilter == -1)
		{
			char filter[] = "";

			//将过滤器加载到已经打开设备，如果打开失败则跳出
			if (pcap_compile(mAdhandle, &fcode, filter, 1, netmask) < 0)
			{
				const char* errormsg = "语法错误，无法编译过滤器";
				strcpy(mErrbuf, errormsg);
				break;
			}
		}
		else
		{
			//保存选择的设备索引
			std::string selfilter = mFilter[selFilter];
			char* filter = new char[selfilter.length() + 1];
			strcpy(filter, selfilter.c_str());
			
			//将过滤器加载到已经打开的设备上面，如果打开失败则跳出
			if (pcap_compile(mAdhandle, &fcode, filter, 1, netmask) < 0)
			{
				const char* errormsg = "语法错误，无法编译过滤器";
				strcpy(mErrbuf, errormsg);
				break;
			}
		}

		//设置过滤器
		if (pcap_setfilter(mAdhandle, &fcode) < 0)
		{
			const char* errormsg = "设置过滤器错误";
			strcpy(mErrbuf, errormsg);
			break;
		}

		//设置自动保存文件
		/*CFileFind file;
		char thistime[30];
		struct tm* ltime;
		memset(mFilepath, 0, 512);
		memset(mFilename, 0, 64);

		if (!file.FindFile(_T("SavedData")))
		{
			CreateDirectory(_T("SavedData"), NULL);
		}

		time_t nowtime;
		time(&nowtime);
		ltime = localtime(&nowtime);
		strftime(thistime, sizeof(thistime), "%Y%m%d %H%M%S", ltime); 
		strcpy(mFilepath, "SavedData\\");
		strcat(mFilename, thistime);
		strcat(mFilename, ".lix");

		strcat(mFilepath, mFilename);
		mDumpfile = pcap_dump_open(mAdhandle, mFilepath);
		if (mDumpfile == NULL)
		{
			const char* errormsg = "文件创建错误";
			strcpy(mErrbuf, errormsg);
			break;
		}*/

		bRet = true;
	} while (false);

	return bRet;
}

/**
* @brief 得到所有过滤器的文本
*
* @return std::vector<CString> 返回所有过滤器的文本
*/
std::vector<CString> Wincap::getFilter()
{
	std::vector<CString> filterArr;

	for (auto var : mFilter)
	{
		filterArr.push_back(exString::MultiCharToWideChar(var).c_str());
	}

	return filterArr;
}

/**
* @brief 
*/
//std::pair<bool, CString> Wincap::getRevData()
//{
//	int res;
//	pcap_pkthdr* header;
//	const u_char* pkt_data = nullptr;
//	u_char *ppkt_data;
//	time_t local_tv_sec;
//	tm *ltime;
//
//	std::pair<bool, CString> bRet = std::make_pair<bool, CString>(false, CString(""));
//
//	res = pcap_next_ex(mAdhandle, &header, &pkt_data);
//
//	do
//	{
//		if (res < 0)
//		{
//			const char* errormsg = "接收数据包错误";
//			strcpy(mErrbuf, errormsg);
//			break;
//		}
//		else if (res == 0)
//		{
//			const char* errormsg = "接收数据包超时";
//			strcpy(mErrbuf, errormsg);
//			break;
//		}
//		else
//		{
//
//			//datapkt* data = new datapkt;
//			datapkt *data = (datapkt*)malloc(sizeof(datapkt));
//			ZeroMemory(data, sizeof(datapkt));
//
//			if (nullptr == data)
//			{
//				const char* errormsg = "空间已满，无法接收新的数据包";
//				strcpy(mErrbuf, errormsg);
//				break;
//			}
//
//			if (Utilities::getInstance()->analyze_frame(pkt_data, data, &(this->mNpacket)) < 0)
//			{
//				const char* errormsg = "解析数据包出错";
//				strcpy(mErrbuf, errormsg);
//				break;
//			}
//
//			if (this->mDumpfile != nullptr)
//			{
//				pcap_dump((unsigned char*)this->mDumpfile, header, pkt_data);
//			}
//
//			//ppkt_data = new u_char;
//			//memcpy(ppkt_data, pkt_data, header->len);
//			//将本地化后的数据装入一个链表中，以便后来使用		
//			ppkt_data = (u_char*)malloc(header->len);
//			memcpy(ppkt_data, pkt_data, header->len);
//
//			this->mnetDataList.AddTail(ppkt_data);
//			this->mlocalDataList.AddTail(data);
//
//			/*预处理，获得时间、长度*/
//			data->len = header->len;								//链路中收到的数据长度
//			local_tv_sec = header->ts.tv_sec;
//			ltime = localtime(&local_tv_sec);
//			data->time[0] = ltime->tm_year + 1900;
//			data->time[1] = ltime->tm_mon + 1;
//			data->time[2] = ltime->tm_mday;
//			data->time[3] = ltime->tm_hour;
//			data->time[4] = ltime->tm_min;
//			data->time[5] = ltime->tm_sec;
//
//			CString str;
//			Utilities::getInstance()->print_packet_hex_str(ppkt_data, data->len, &str);
//
//			bRet.first = true;
//			bRet.second = str;
//
//			if (data->ethh != nullptr)
//			{
//				delete data->ethh;
//			}
//
//			if (data->arph != nullptr)
//			{
//				delete data->arph;
//			}
//
//			if (data->iph != nullptr)
//			{
//				delete data->iph;
//			}
//
//			if (data->iph6 != nullptr)
//			{
//				delete data->iph6;
//			}
//
//			if (data->icmph != nullptr)
//			{
//				delete data->icmph;
//			}
//
//			if (data->icmph6 != nullptr)
//			{
//				delete data->icmph6;
//			}
//
//			if (data->tcph != nullptr)
//			{
//				delete data->tcph;
//			}
//
//			if (data->udph != nullptr)
//			{
//				delete data->udph;
//			}
//
//			free(ppkt_data);
//			free(data);
//		}
//	} while (false);
//
//	return bRet;
//}

/**
* @brief 得到截取到的网络数据包
*
* @return std::pair<bool, CString> 返回数据包内容，第一个参数为true时表示截取成功，为false时表示截取失败
*/
std::pair<bool, CString> Wincap::getRevDataClean()
{
	int res;
	pcap_pkthdr* header;
	const u_char* pkt_data = nullptr;
	u_char *ppkt_data;
	time_t local_tv_sec;
	tm *ltime;

	std::pair<bool, CString> bRet = std::make_pair<bool, CString>(false, CString(""));

	//得到数据包
	res = pcap_next_ex(mAdhandle, &header, &pkt_data);

	do
	{
		//如果小于零表示接收数据包出错
		if (res < 0)
		{
			const char* errormsg = "接收数据包错误";
			strcpy(mErrbuf, errormsg);
			break;
		}
		//如果等于零表示接收数据包出错
		else if (res == 0)
		{
			const char* errormsg = "接收数据包超时";
			strcpy(mErrbuf, errormsg);
			break;
		}
		else
		{

			//datapkt* data = new datapkt;
			datapkt *data = (datapkt*)malloc(sizeof(datapkt));
			ZeroMemory(data, sizeof(datapkt));

			//如果为空表示申请内存失败，执行if语句
			if (nullptr == data)
			{
				const char* errormsg = "空间已满，无法接收新的数据包";
				strcpy(mErrbuf, errormsg);
				break;
			}

			//解析数据包，并保存到data以及mNpacket中
			if (Utilities::getInstance()->analyze_frame(pkt_data, data, &(this->mNpacket)) < 0)
			{
				const char* errormsg = "解析数据包出错";
				strcpy(mErrbuf, errormsg);
				break;
			}

			//如果保存文件的索引存在，那么将数据进行保存
			if (this->mDumpfile != nullptr)
			{
				pcap_dump((unsigned char*)this->mDumpfile, header, pkt_data);
			}

			//ppkt_data = new u_char;
			//memcpy(ppkt_data, pkt_data, header->len);
			//将本地化后的数据装入一个链表中，以便后来使用		
			ppkt_data = (u_char*)malloc(header->len);
			memcpy(ppkt_data, pkt_data, header->len);

			/*预处理，获得时间、长度*/
			data->len = header->len;								//链路中收到的数据长度
			local_tv_sec = header->ts.tv_sec;
			ltime = localtime(&local_tv_sec);
			data->time[0] = ltime->tm_year + 1900;
			data->time[1] = ltime->tm_mon + 1;
			data->time[2] = ltime->tm_mday;
			data->time[3] = ltime->tm_hour;
			data->time[4] = ltime->tm_min;
			data->time[5] = ltime->tm_sec;

			//将数据包中的数据解析成字符串
			CString str;
			Utilities::getInstance()->print_packet_hex_str(ppkt_data, data->len, &str);

			bRet.first = true;
			bRet.second = str;

			//释放内存
			if (data->ethh != nullptr)
			{
				delete data->ethh;
			}

			if (data->arph != nullptr)
			{
				delete data->arph;
			}

			if (data->iph != nullptr)
			{
				delete data->iph;
			}

			if (data->iph6 != nullptr)
			{
				delete data->iph6;
			}

			if (data->icmph != nullptr)
			{
				delete data->icmph;
			}

			if (data->icmph6 != nullptr)
			{
				delete data->icmph6;
			}

			if (data->tcph != nullptr)
			{
				delete data->tcph;
			}

			if (data->udph != nullptr)
			{
				delete data->udph;
			}

			free(ppkt_data);
			free(data);

		}
	} while (false);

	return bRet;
}


