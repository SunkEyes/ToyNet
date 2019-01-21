#include "buffer.h"

//////////////////////////////////////////////////////////////////////////
packet_buffer::packet_buffer(int buff_len)
{
	_head = _tail = 0;
	_packets.clear();

	max_buff_len = buff_len;
	_data = new char[buff_len];
}

packet_buffer::~packet_buffer()
{
	delete[] _data;
	_data = NULL;
}

bool packet_buffer::empty()
{
	return _packets.size() == 0;
}

bool packet_buffer::pop_packet(char* data, int& len, IP_ADDRESS* peer_ip, unsigned short* peer_port)
{
	auto& bi = _packets[0];
	// 用户传进的缓冲不够
	if (bi.len > len)
		return false;

	len = bi.len;
	memcpy(data, _data + bi.start, len);
	_packets.erase(_packets.begin());

	_head = bi.start + bi.len;
	if (peer_ip) *peer_ip = bi.peer_ip;
	if (peer_port) *peer_port = bi.peer_port;

	return true;
}

bool packet_buffer::push_packet(const void* data, int len, IP_ADDRESS peer_ip, unsigned short peer_port)
{
	int startPos = _tail;

	if (_tail > _head)
	{
		if (max_buff_len - _tail < len)	// 尾部剩余不够了，跳过尾部
		{
			if (_head < len)	// 头部数据也不够存data，则认为缓冲区已满，丢掉这个包
			{
				return false;
			}
			startPos = 0;	// 存在头部
		}
	}
	else if (_packets.size() > 0 && _head - _tail < len)
	{
		return false;
	}

	memcpy(_data + startPos, data, len);
	_tail = startPos + len;
	packet_range bi{ startPos, len, peer_ip, peer_port };
	_packets.push_back(bi);

	return true;
}