#pragma once
#include <array>
#include <vector>
#include <pcap.h>
#include "Protocol.h"

/**
* @brief ���һ����ȡ���������
*/
class Wincap
{
protected:
	bool mbSaveFile          = false;
	int mDevCount            = 0;           //�洢ɨ�赽���ٸ��豸
	pcap_t*    mAdhandle     = nullptr;     //�洢��ǰ�򿪵������豸
	pcap_if_t* mAlldev       = nullptr;     //��������ɨ�赽���豸����
	pcap_dumper_t* mDumpfile = nullptr;     //�������ñ��浽���ص��ļ�����
	pktcount mNpacket;                      //�������ݰ���ͳ��
	CPtrList mlocalDataList;				//���汻���ػ�������ݰ�
	CPtrList mnetDataList;			    //�����������ֱ�ӻ�ȡ�����ݰ�
	char mErrbuf[PCAP_ERRBUF_SIZE];         //���������Ϣ
	std::array<std::string, 5> mFilter;     //���������
	char mFilepath[512];					//�ļ�����·��
	char mFilename[64];						//�ļ�����		

protected:
	Wincap();

public:
	~Wincap();

public:
	//��ʼ���豸
	bool initCap(bool bSave = false);

	//�ͷ��豸
	void freeCap();

	//�õ����е��豸
	std::vector<pcap_if_t*> getAllDevice();

	//���ƶ����豸
	bool OpenDevice(int selIndex, int selFilter = -1);

	//�رմ򿪵��豸
	void CloseDevice();

	//�õ�ִ�д������Ϣ
	std::wstring getErrMsg();

	//�õ�������
	std::vector<std::wstring> getFilter();

	//�õ����յ�������
	std::pair<bool, CString> getRevData();

	//�õ������ͷ��ڴ������
	std::pair<bool, std::wstring> getRevDataClean();

	//�õ�׽����ʵ������
	static Wincap* getInstance();
};

