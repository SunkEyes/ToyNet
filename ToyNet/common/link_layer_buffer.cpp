
#include "link_layer_buffer.h"

bool data_buffer::pop_packet(char* data, int& len)
{
	pthread_mutex_lock(&_lock);
	if (_buffer->empty())
		pthread_cond_wait(&_not_empty, &_lock);

	bool ret = _buffer->pop_packet(data, len);

	pthread_mutex_unlock(&_lock);
	return ret;
}

bool data_buffer::push_packet(const void* data, int len)
{
	pthread_mutex_lock(&_lock);
	bool ret = _buffer->push_packet(data, len);
	pthread_cond_signal(&_not_empty);
	pthread_mutex_unlock(&_lock);

	return ret;
}