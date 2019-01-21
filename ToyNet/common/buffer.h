#pragma once

#include <vector>
#include <map>
#include "common_def.h"
#include "network_layer/net_layer.h"


struct packet_range
{
	int start;
	int len;
	// ���������ip������Э��Ľ��ջ�����Ҫ��
	IP_ADDRESS peer_ip;
	unsigned short peer_port;
};

// ����β��ѭ�����ݿ�
struct packet_buffer
{
private:
	int max_buff_len;
	char *_data;
	int _head, _tail;

	// ���ݱ߽�
	std::vector<packet_range> _packets;
	
public:
	packet_buffer(int buff_len);
	~packet_buffer();

	bool empty();

	// ��·��ʹ��ʱ��ip��port��������
	bool pop_packet(char* data, int& len, IP_ADDRESS* peer_ip = NULL, unsigned short* peer_port = NULL);
	bool push_packet(const void* data, int len, IP_ADDRESS peer_ip = 0, unsigned short peer_port = 0);
};