#include "stdafx.h"
#include "exString.h"
#include "Wincap.h"
#include "Utilities.h"

Wincap::Wincap()
{
	//��ʼ��������Ϣ����
	ZeroMemory(mErrbuf, PCAP_ERRBUF_SIZE);

	//��ʼ��������
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
* @brief �õ�׽��ʵ��
*
* @return ����׽��ʵ��ָ��
*/
Wincap* Wincap::getInstance(){
	static Wincap mSelf;

	return &mSelf;
}

/**
* @brief ��ʼ���豸
*
* @return ����true��ʾ��ʼ���ɹ�������false��ʾ��ʼ��ʧ�ܣ�ʹ�ú����õ�������Ϣ
*/
bool Wincap::initCap()
{
	//����Ѿ��ҵ��豸����ô���ͷ�
	if (mDevCount > 0)
	{
		freeCap();
	}

	//����pcap_findalldevsɨ�����������豸
	if (pcap_findalldevs(&mAlldev, mErrbuf) == -1)
	{
		return false;
	}

	//ѭ��ɨ��ÿһ���豸���������е��豸����
	for (pcap_if_t* dev = mAlldev; dev != nullptr; dev = dev->next)
	{
		mDevCount++;
	}

	return true;
}

/**
* @brief �ͷ�����ɨ�赽���豸
*/
void Wincap::freeCap()
{
	//����豸���������㣬��ʾɨ�赽�豸����ô����pcap_freealldevs�ͷ��豸
	if (mDevCount > 0)
	{
		pcap_freealldevs(mAlldev);
		mDevCount = 0;
	}
}

/**
* @brief ��������ɨ�赽���豸
*
* @return std::vector<pcap_if_t*> ��������ɨ�赽�豸������
*/
std::vector<pcap_if_t*> Wincap::getAllDevice()
{
	std::vector<pcap_if_t*> devBuff;

	//�������豸ɨ��һ�飬�洢�ڻ����з���
	for (pcap_if_t* dev = mAlldev; dev != nullptr; dev = dev->next)
	{
		devBuff.push_back(dev);
	}

	return devBuff;
}

/**
* @brief ���ش�����Ϣ
*/
CString Wincap::getErrMsg()
{
	return CString(exString::MultiCharToWideChar(mErrbuf).c_str());
}

/**
* @brief ���ƶ��������豸
*
* @param int selIndex ������Ҫ���豸�����������������0��ʼ
* @param int selFilter �������������Ĭ��ֵΪ-1����ʾ��ȡ�������͵İ�
*
* @return bool ����true��ʾ�򿪳ɹ�������false��ʾ��ʧ��
*/
bool Wincap::OpenDevice(int selIndex, int selFilter/* = -1*/)
{
	bool bRet = false;
	u_int netmask;
	struct bpf_program fcode;

	do{
		//�ж����������Ƿ���ϣ����������������
		if ((selIndex < 0 || selIndex >= mDevCount) ||
			(selFilter != -1 && (selFilter < 0 || selFilter >= mFilter.size())))
		{
			const char* errormsg = "������������ȷ�Ϻ��ٳ���";
			strcpy(mErrbuf, errormsg);
			break;
		}

		pcap_if_t* seldev = nullptr;
		pcap_if_t* dev = mAlldev;

		//���������ҵ���Ҫ�򿪵��豸
		for (int i = 0; i < selIndex; i++)
		{
			dev = dev->next;
		}

		seldev = dev;

		//����Ҫ�򿪵��豸
		if ((mAdhandle = pcap_open_live(seldev->name, 65536, 1, 1000, mErrbuf)) == nullptr)
		{
			const char* errormsg = "��ָ�����豸������ȷ�Ϻ��ٳ���";
			strcpy(mErrbuf, errormsg);
			break;
		}

		//�����ݰ�����
		if (pcap_datalink(mAdhandle) != DLT_EN10MB)
		{
			const char* errormsg = "�ⲻ�ʺ��ڷ���̫��������";
			strcpy(mErrbuf, errormsg);
			break;
		}

		//������豸�ĵ�ַ���ڣ���ô���������Ĭ��ֵ
		if (dev->addresses != NULL)
			netmask = ((struct sockaddr_in *)(dev->addresses->netmask))->sin_addr.S_un.S_addr;
		else
			netmask = 0xffffff;

		//�������Ĺ�������-1����ô��ʾ�������͵İ�����Ҫץȡ
		if (selFilter == -1)
		{
			char filter[] = "";

			//�����������ص��Ѿ����豸�������ʧ��������
			if (pcap_compile(mAdhandle, &fcode, filter, 1, netmask) < 0)
			{
				const char* errormsg = "�﷨�����޷����������";
				strcpy(mErrbuf, errormsg);
				break;
			}
		}
		else
		{
			//����ѡ����豸����
			std::string selfilter = mFilter[selFilter];
			char* filter = new char[selfilter.length() + 1];
			strcpy(filter, selfilter.c_str());
			
			//�����������ص��Ѿ��򿪵��豸���棬�����ʧ��������
			if (pcap_compile(mAdhandle, &fcode, filter, 1, netmask) < 0)
			{
				const char* errormsg = "�﷨�����޷����������";
				strcpy(mErrbuf, errormsg);
				break;
			}
		}

		//���ù�����
		if (pcap_setfilter(mAdhandle, &fcode) < 0)
		{
			const char* errormsg = "���ù���������";
			strcpy(mErrbuf, errormsg);
			break;
		}

		//�����Զ������ļ�
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
			const char* errormsg = "�ļ���������";
			strcpy(mErrbuf, errormsg);
			break;
		}*/

		bRet = true;
	} while (false);

	return bRet;
}

/**
* @brief �õ����й��������ı�
*
* @return std::vector<CString> �������й��������ı�
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
//			const char* errormsg = "�������ݰ�����";
//			strcpy(mErrbuf, errormsg);
//			break;
//		}
//		else if (res == 0)
//		{
//			const char* errormsg = "�������ݰ���ʱ";
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
//				const char* errormsg = "�ռ��������޷������µ����ݰ�";
//				strcpy(mErrbuf, errormsg);
//				break;
//			}
//
//			if (Utilities::getInstance()->analyze_frame(pkt_data, data, &(this->mNpacket)) < 0)
//			{
//				const char* errormsg = "�������ݰ�����";
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
//			//�����ػ��������װ��һ�������У��Ա����ʹ��		
//			ppkt_data = (u_char*)malloc(header->len);
//			memcpy(ppkt_data, pkt_data, header->len);
//
//			this->mnetDataList.AddTail(ppkt_data);
//			this->mlocalDataList.AddTail(data);
//
//			/*Ԥ�������ʱ�䡢����*/
//			data->len = header->len;								//��·���յ������ݳ���
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
* @brief �õ���ȡ�����������ݰ�
*
* @return std::pair<bool, CString> �������ݰ����ݣ���һ������Ϊtrueʱ��ʾ��ȡ�ɹ���Ϊfalseʱ��ʾ��ȡʧ��
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

	//�õ����ݰ�
	res = pcap_next_ex(mAdhandle, &header, &pkt_data);

	do
	{
		//���С�����ʾ�������ݰ�����
		if (res < 0)
		{
			const char* errormsg = "�������ݰ�����";
			strcpy(mErrbuf, errormsg);
			break;
		}
		//����������ʾ�������ݰ�����
		else if (res == 0)
		{
			const char* errormsg = "�������ݰ���ʱ";
			strcpy(mErrbuf, errormsg);
			break;
		}
		else
		{

			//datapkt* data = new datapkt;
			datapkt *data = (datapkt*)malloc(sizeof(datapkt));
			ZeroMemory(data, sizeof(datapkt));

			//���Ϊ�ձ�ʾ�����ڴ�ʧ�ܣ�ִ��if���
			if (nullptr == data)
			{
				const char* errormsg = "�ռ��������޷������µ����ݰ�";
				strcpy(mErrbuf, errormsg);
				break;
			}

			//�������ݰ��������浽data�Լ�mNpacket��
			if (Utilities::getInstance()->analyze_frame(pkt_data, data, &(this->mNpacket)) < 0)
			{
				const char* errormsg = "�������ݰ�����";
				strcpy(mErrbuf, errormsg);
				break;
			}

			//��������ļ����������ڣ���ô�����ݽ��б���
			if (this->mDumpfile != nullptr)
			{
				pcap_dump((unsigned char*)this->mDumpfile, header, pkt_data);
			}

			//ppkt_data = new u_char;
			//memcpy(ppkt_data, pkt_data, header->len);
			//�����ػ��������װ��һ�������У��Ա����ʹ��		
			ppkt_data = (u_char*)malloc(header->len);
			memcpy(ppkt_data, pkt_data, header->len);

			/*Ԥ�������ʱ�䡢����*/
			data->len = header->len;								//��·���յ������ݳ���
			local_tv_sec = header->ts.tv_sec;
			ltime = localtime(&local_tv_sec);
			data->time[0] = ltime->tm_year + 1900;
			data->time[1] = ltime->tm_mon + 1;
			data->time[2] = ltime->tm_mday;
			data->time[3] = ltime->tm_hour;
			data->time[4] = ltime->tm_min;
			data->time[5] = ltime->tm_sec;

			//�����ݰ��е����ݽ������ַ���
			CString str;
			Utilities::getInstance()->print_packet_hex_str(ppkt_data, data->len, &str);

			bRet.first = true;
			bRet.second = str;

			//�ͷ��ڴ�
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


