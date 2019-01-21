#pragma once
#include "buffer.h"
#include "pthread.h"

// 暂时继承packet_buffer，网卡拿到的数据直接右边界的，以后改为字节流
struct data_buffer
{
public:

	data_buffer(int buff_size)
	{
		_buffer = new packet_buffer(buff_size);
		pthread_mutex_init(&_lock, 0);
		pthread_cond_init(&_not_empty, 0);
	}
	~data_buffer()
	{
		delete _buffer;
		_buffer = NULL;
	}

	bool pop_packet(char* data, int& len);
	bool push_packet(const void* data, int len);

private:
	pthread_mutex_t _lock;
	pthread_cond_t _not_empty;
	packet_buffer* _buffer;
};