#pragma once
#include <memory>
#include <string>
#include <WinSock2.h>

using std::shared_ptr;

class Utilities
{
protected:
	static shared_ptr<Utilities> mSelf;

private:
	Utilities();

public:
	~Utilities();

	/*��·���*/
	int analyze_frame(const u_char * pkt, struct datapkt * data, struct pktcount *npakcet);

	/*������*/
	int analyze_ip(const u_char* pkt, struct datapkt *data, struct pktcount *npakcet);
	int analyze_ip6(const u_char* pkt, struct datapkt *data, struct pktcount *npakcet);
	int analyze_arp(const u_char* pkt, struct datapkt *data, struct pktcount *npakcet);

	/*������*/
	int analyze_icmp(const u_char* pkt, struct datapkt *data, struct pktcount *npakcet);
	//icmp��Ϊ���⣬ͨ����Ϊ����ip��һ���ݣ�����������Ϊip������Ч��
	//�ɵ���ʽ�������д��䣬��tcp��udpһ����������������ڴ����������
	int analyze_icmp6(const u_char* pkt, struct datapkt *data, struct pktcount *npakcet);

	int analyze_tcp(const u_char* pkt, struct datapkt *data, struct pktcount *npakcet);
	int analyze_udp(const u_char* pkt, struct datapkt *dtat, struct pktcount *npakcet);

	/*Ӧ�ò��*/
	int analyze_http(const u_char* pkt, struct datapkt *data, struct pktcount *npakcet);

	/*�����ݰ���ʮ�����Ƶķ�ʽ��ӡ*/
	void print_packet_hex(const u_char* pkt, int size_pkt, CString *buf);
	void print_packet_hex_str(const u_char* pkt, int size_pkt, std::wstring* wbuf);
	CString UTF8ToUnicode(char* UTF8);
	static Utilities* getInstance();
};

