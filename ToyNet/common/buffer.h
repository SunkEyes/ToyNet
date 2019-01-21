#pragma once

#include <vector>
#include <map>
#include "common_def.h"
#include "network_layer/net_layer.h"


struct packet_range
{
	int start;
	int len;
	// 以下两项，在ip及以上协议的接收缓冲中要存
	IP_ADDRESS peer_ip;
	unsigned short peer_port;
};

// 不跨尾的循环数据块
struct packet_buffer
{
private:
	int max_buff_len;
	char *_data;
	int _head, _tail;

	// 数据边界
	std::vector<packet_range> _packets;
	
public:
	packet_buffer(int buff_len);
	~packet_buffer();

	bool empty();

	// 链路层使用时，ip和port都不用填
	bool pop_packet(char* data, int& len, IP_ADDRESS* peer_ip = NULL, unsigned short* peer_port = NULL);
	bool push_packet(const void* data, int len, IP_ADDRESS peer_ip = 0, unsigned short peer_port = 0);
};