#include "stdafx.h"
#include "Utilities.h"
#include <afxcmn.h>
#include <afxwin.h>
#include "Protocol.h"

shared_ptr<Utilities> Utilities::mSelf = nullptr;

Utilities::Utilities()
{
}


Utilities::~Utilities()
{
}

Utilities* Utilities::getInstance()
{
	if (mSelf.use_count() == 0)
	{
		mSelf = shared_ptr<Utilities>(new Utilities());
	}

	return mSelf.get();
}

/**
* @brief �������ݰ�
*
* @param const u_char* pkt �����ȡ�������ݰ�
* @param datapkt* data     ���뱣�����ݰ��Ļ���
* @param pktcount* npacket ���뱣��������ݰ�����ͳ�ƻ���
*
* @return int ����-1��ʾ����ʧ�ܣ�����1��ʾ�����ɹ�
*/
int Utilities::analyze_frame(const u_char* pkt, datapkt* data, pktcount* npacket)
{
	ethhdr* ethh = (ethhdr*)pkt;
	data->ethh = new ethhdr;

	if (nullptr == data->ethh)
	{
		return -1;
	}

	//�������ݰ��е�����
	for (int i = 0; i < 6; i++)
	{
		data->ethh->dest[i] = ethh->dest[i];
		data->ethh->src[i] = ethh->src[i];
	}

	//�����ܹ����ݰ�����
	npacket->n_sum++;

	//�������ֽ�˳��ת��Ϊ�����ֽ�˳��
	data->ethh->type = ntohs(ethh->type);

	//���ݲ�ͬ������ת�벻ͬ�����ݰ�����
	switch (data->ethh->type)
	{
	case 0x0806:
		return analyze_arp((u_char*)pkt + 14, data, npacket);
		break;

	case 0x0800:
		return analyze_ip((u_char*)pkt + 14, data, npacket);
		break;

	case 0x86dd:
		return analyze_ip6((u_char*)pkt + 14, data, npacket);
		break;

	default:
		npacket->n_other++;
		return -1;
		break;
	}

	return 1;
}

/**
* @brief ����arp���ݰ�
*
* @param const u_char* pkt �������ݰ�
* @param datapkt* data     ���뱣�滺��
* @param pktcount* npacket �������ݰ�����ͳ��
*
* @return int ����-1��ʾ����ʧ�ܣ�����1��ʾ�����ɹ�
*/
int Utilities::analyze_arp(const u_char* pkt, datapkt* data, pktcount* npacket)
{
	arphdr* arph = (arphdr*)pkt;
	data->arph = new arphdr();

	if (nullptr == data->arph)
	{
		return -1;
	}

	//����ip���ݰ��Լ�mac���ݰ�
	for (int i = 0; i < 6; i++)
	{
		if (i < 4)
		{
			data->arph->ar_destip[i] = arph->ar_destip[i];
			data->arph->ar_srcip[i] = arph->ar_srcip[i];
		}

		data->arph->ar_destmac[i] = arph->ar_destmac[i];
		data->arph->ar_srcmac[i] = arph->ar_srcmac[i];
	}

	//����arp���ݰ�
	data->arph->ar_hln = arph->ar_hln;
	data->arph->ar_hrd = ntohs(arph->ar_hrd);
	data->arph->ar_op = ntohs(arph->ar_op);
	data->arph->ar_pln = arph->ar_pln;
	data->arph->ar_pro = ntohs(arph->ar_pro);

	strcpy(data->pktType, "ARP");
	npacket->n_arp++;
	return 1;
}

/**
* @brief ����ip���ݰ�
*
* @param const u_char* pkt ������Ҫ���������ݰ�
* @param datapkt* data     ���뱣��������ݰ��Ļ���
* @param pktcount* npacket ���뱣�治ͬ�������ݰ���ͳ��
*
* @return int ����-1��ʾ����ʧ�ܣ�����1��ʾ�����ɹ�
*/
int Utilities::analyze_ip(const u_char* pkt, datapkt* data, pktcount* npacket)
{
	iphdr* iph = (iphdr*)pkt;
	data->iph = new iphdr();

	if (nullptr == data->iph)
	{
		return -1;
	}

	//�����ݰ�������������
	data->iph->check = iph->check;
	npacket->n_ip++;

	data->iph->saddr = iph->saddr;
	data->iph->daddr = iph->daddr;

	data->iph->frag_off = iph->frag_off;
	data->iph->id = iph->id;
	data->iph->proto = iph->proto;
	data->iph->tlen = iph->tlen;
	data->iph->tos = iph->tos;
	data->iph->ttl = iph->ttl;
	data->iph->ihl = iph->ihl;
	data->iph->version = iph->version;
	data->iph->op_pad = iph->op_pad;

	//���ݲ�ͬ���ͽ�����ͬ�����ݰ�
	int iplen = iph->ihl * 4;
	switch (iph->proto)
	{
	case PROTO_ICMP:
		return analyze_icmp((u_char*)iph + iplen, data, npacket);
		break;

	case PROTO_TCP:
		return analyze_tcp((u_char*)iph + iplen, data, npacket);
		break;

	case PROTO_UDP:
		return analyze_udp((u_char*)iph + iplen, data, npacket);
		break;

	default:
		return -1;
		break;
	}

	return 1;
}

/**
* @brief ����IPV6���ݰ�
*
* @param const u_char* pkt �������ݰ�
* @param datapkt* data     ���뱣�����ݰ��Ļ���
* @param pktcount* npacket ����ͳ�Ƹ������ݰ��Ļ���
*
* @return int ����-1��ʾ����ʧ�ܣ�����1��ʾ�����ɹ�
*/
int Utilities::analyze_ip6(const u_char* pkt, datapkt* data, pktcount* npacket)
{
	iphdr6* iph6 = (iphdr6*)pkt;
	data->iph6 = new iphdr6;

	if (nullptr == data->iph6)
	{
		return -1;
	}

	npacket->n_ip6++;

	//����ipv6������
	data->iph6->version = iph6->version;
	data->iph6->flowtype = iph6->flowtype;
	data->iph6->flowid = iph6->flowid;
	data->iph6->plen = iph6->plen;
	data->iph6->nh = iph6->nh;
	data->iph6->hlim = iph6->hlim;

	for (int i = 0; i < 16; i++)
	{
		data->iph6->saddr[i] = iph6->saddr[i];
	}

	//���ݲ�ͬ�����͵��ò�ͬ�Ľ�����ʽ
	switch (iph6->nh)
	{
	case 0x3a:
		return analyze_icmp6((u_char*)iph6 + 40, data, npacket);
		break;

	case 0x06:
		return analyze_tcp((u_char*)iph6 + 40, data, npacket);
		break;

	case 0x11:
		return analyze_udp((u_char*)iph6 + 40, data, npacket);
		break;

	default:
		return -1;
		break;
	}

	return 1;
}

/**
* @brief 
*/
int Utilities::analyze_icmp(const u_char* pkt, datapkt* data, pktcount* npacket)
{
	icmphdr* icmph = (icmphdr*)pkt;
	data->icmph = new icmphdr;

	if (nullptr == data->icmph)
	{
		return -1;
	}

	data->icmph->chksum = icmph->chksum;
	data->icmph->code = icmph->code;
	data->icmph->seq = icmph->seq;
	data->icmph->type = icmph->type;
	strcpy(data->pktType, "ICMP");
	npacket->n_icmp++;

	return 1;
}

int Utilities::analyze_icmp6(const u_char* pkt, datapkt* data, pktcount* npacket)
{
	icmphdr6* icmph6 = (icmphdr6*)pkt;
	data->icmph6 = new icmphdr6;

	if (nullptr == data->icmph6)
	{
		return -1;
	}

	data->icmph6->chksum = icmph6->chksum;
	data->icmph6->code = icmph6->code;
	data->icmph6->seq = icmph6->seq;
	data->icmph6->type = icmph6->type;
	data->icmph6->op_len = icmph6->op_len;
	data->icmph6->op_type = icmph6->op_type;

	for (int i = 0; i < 6; i++)
	{
		data->icmph6->op_ethaddr[i] = icmph6->op_ethaddr[i];
	}

	strcpy(data->pktType, "ICMPv6");
	npacket->n_icmp6++;

	return 1;
}

int Utilities::analyze_tcp(const u_char* pkt, datapkt* data, pktcount* npacket)
{
	tcphdr* tcph = (tcphdr*)pkt;
	data->tcph = new tcphdr;
	if (nullptr == data->tcph)
	{
		return -1;
	}

	data->tcph->ack_seq = tcph->ack_seq;
	data->tcph->check = tcph->check;
	data->tcph->doff = tcph->doff;
	data->tcph->res1 = tcph->res1;
	data->tcph->cwr = tcph->cwr;
	data->tcph->ece = tcph->ece;
	data->tcph->urg = tcph->urg;
	data->tcph->ack = tcph->ack;
	data->tcph->psh = tcph->psh;
	data->tcph->rst = tcph->rst;
	data->tcph->syn = tcph->syn;
	data->tcph->fin = tcph->fin;

	data->tcph->dport = ntohs(tcph->dport);
	data->tcph->seq = tcph->seq;
	data->tcph->sport = ntohs(tcph->sport);
	data->tcph->urg_ptr = tcph->urg_ptr;
	data->tcph->window = tcph->window;
	data->tcph->opt = tcph->opt;

	if (ntohs(tcph->dport) == 80 || ntohs(tcph->sport) == 80)
	{
		npacket->n_http++;
		strcpy(data->pktType, "HTTP");
	}
	else
	{
		npacket->n_tcp++;
		strcpy(data->pktType, "TCP");
	}

	return 1;
}

int Utilities::analyze_udp(const u_char* pkt, datapkt* data, pktcount* npacket)
{
	udphdr* udph = (udphdr*)pkt;
	data->udph = new udphdr;
	
	if (nullptr == data->udph)
	{
		return -1;
	}

	data->udph->check = udph->check;
	data->udph->dport = ntohs(udph->dport);
	data->udph->len = ntohs(udph->len);
	data->udph->sport = ntohs(udph->sport);

	strcpy(data->pktType, "UDP");
	npacket->n_udp++;

	return 1;
}

void Utilities::print_packet_hex(const u_char* pkt, int size_pkt, CString* buf)
{
	u_char ch;
	char tempbuf[256];
	memset(tempbuf, 0, 256);

	for (int i = 0, rowcount; i < size_pkt; i += 16)
	{
		buf->AppendFormat(_T("%04x: "), (u_int)i);
		rowcount = (size_pkt - i) > 16 ? 16 : (size_pkt - i);

		for (int j = 0; j < rowcount; j++)
		{
			buf->AppendFormat(_T("%02x "), (u_int)pkt[i + j]);
		}

		if (rowcount < 16)
		{
			for (int j = rowcount; j < 16; j++)
			{
				buf->AppendFormat(_T("     "));
			}
		}

		for (int j = 0; j < rowcount; j++)
		{
			ch = pkt[i + j];
			ch = isprint(ch) ? ch : '.';
			buf->AppendFormat(_T("%c"), ch);
		}

		buf->Append(_T("\r\n"));

		if (rowcount < 16)
		{
			return;
		}
	}
}

/**
* @brief �����ݰ����������еõ��ı�����
*
* @param const u_char* pkt ������Ҫ���������ݰ�
* @param int size_pkt �������ݰ��ĳ���
* @param CString* buf ���뱣��������ݵĻ����ַ
*/
void Utilities::print_packet_hex_str(const u_char* pkt, int size_pkt, CString* buf)
{
	u_char ch;
	char tempbuf[2048];
	memset(tempbuf, 0, 2048);
	CString bufStr;
	bool isName = true;

	//��ʼ����ÿһ���ֽ�
	for (int i = 0, j = 0; i < size_pkt; i++)
	{
		//�����ж��Ƿ��ҵ��û������ҵ��û�����ʼ�������û������û�����UTF8�ı��룩
		//if (isName && (buf->Find(_T("fromname\":\""))) != -1)
		//{
		//	ch = pkt[i];
		//	bufStr.AppendFormat(_T("%c"), ch);
		//	if (bufStr == _T("\""))
		//	{
		//		isName = false;

		//		CString fromName = UTF8ToUnicode(tempbuf);
		//		buf->Append(fromName);

		//		goto next;
		//	}
		//	else
		//	{
		//		tempbuf[j] = ch;
		//		j++;
		//	}
		//	bufStr = _T("");
		//}
		//else
		//		{
		//next:
		ch = pkt[i];
		ch = isprint(ch) ? ch : '.';
		buf->AppendFormat(_T("%c"), ch);
		//		}

		if (buf->Find(_T("{")) != -1)
		{
			for (; i < size_pkt; i++)
			{
				unsigned char var = pkt[i];
				if (var == 125)
				{
					tempbuf[j] = var;
					break;
				}
				tempbuf[j++] = var;
			}

			char* pfind = strstr(tempbuf, "fromname");

			if (pfind != NULL)
			{
				*buf = UTF8ToUnicode(tempbuf);
			}

			break;
		}
	}
}

CString Utilities::UTF8ToUnicode(char* UTF8)

{

	DWORD dwUnicodeLen;        //ת����Unicode�ĳ���

	TCHAR *pwText;            //����Unicode��ָ��

	CString strUnicode;        //����ֵ

	//���ת����ĳ��ȣ��������ڴ�

	dwUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, UTF8, -1, NULL, 0);

	pwText = new TCHAR[dwUnicodeLen + 1];

	if (!pwText)

	{

		return strUnicode;

	}

	//תΪUnicode

	MultiByteToWideChar(CP_UTF8, 0, UTF8, -1, pwText, dwUnicodeLen);

	pwText[dwUnicodeLen] = L'\0';
	//תΪCString

	strUnicode.Format(_T("%s"), pwText);

	//����ڴ�

	delete[]pwText;

	//����ת���õ�Unicode�ִ�

	return strUnicode;

}
