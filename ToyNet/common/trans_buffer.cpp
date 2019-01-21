#include "trans_buffer.h"
#include "arp_cache.h"
#include "network_layer\arp_imp.h"


//////////////////////////////////////////////////////////////////////////
bool trans_recv_buffer::pop_packet(char* data, int& len, IP_ADDRESS& local_ip, unsigned short& local_port)
{
	pthread_mutex_lock(&_lock);

	IP_PORT ipp = MAKE_UINT64(local_ip, local_port);
	auto it = _buffers.find(ipp);
	if (it == _buffers.end())
	{
		// 连接异常
		//set_toy_errno()
		pthread_mutex_unlock(&_lock);
		return false;
	}

	if (it->second->empty())
		pthread_cond_wait(&_exist_packets, &_lock);

	bool ret = it->second->pop_packet(data, len, &local_ip, &local_port);
	pthread_mutex_unlock(&_lock);
	return ret;
}

bool trans_recv_buffer::push_packet(const void* data, int len, 
									IP_ADDRESS peer_ip, unsigned short peer_port, 
									IP_ADDRESS local_ip, unsigned short local_port)
{
	pthread_mutex_lock(&_lock);

	IP_PORT ipp = MAKE_UINT64(local_ip, local_port);
	auto it = _buffers.find(ipp);
	// 没人在监听这个端口，丢弃
	if (it == _buffers.end())
	{
		pthread_mutex_unlock(&_lock);
		return false;
	}

	it->second->push_packet(data, len, peer_ip, peer_port);

	pthread_cond_signal(&_exist_packets);
	pthread_mutex_unlock(&_lock);
	return true;
}

bool trans_recv_buffer::try_erase(IP_ADDRESS ip, unsigned short port)
{
	pthread_mutex_lock(&_lock);
	UINT64 ip_port = MAKE_UINT64(ip, port);
	auto it = _buffers.find(ip_port);
	if (it == _buffers.end())
	{
		pthread_mutex_unlock(&_lock);
		return false;
	}

	delete it->second;
	_buffers.erase(it);
	pthread_mutex_unlock(&_lock);

	return true;
}

bool trans_recv_buffer::has(IP_ADDRESS ip, unsigned short port)
{
	pthread_mutex_lock(&_lock);
	UINT64 ip_port = MAKE_UINT64(ip, port);
	auto it = _buffers.find(ip_port);
	bool found = (it != _buffers.end());
	pthread_mutex_unlock(&_lock);

	return found;
}

void trans_recv_buffer::add(IP_ADDRESS ip, unsigned short port, int buff_size)
{
	pthread_mutex_lock(&_lock);
	UINT64 ip_port = MAKE_UINT64(ip, port);
	if (_buffers.find(ip_port) != _buffers.end())
	{
		pthread_mutex_unlock(&_lock);
		return;
	}
	_buffers[ip_port] = new packet_buffer(buff_size);
	pthread_mutex_unlock(&_lock);
}

bool trans_recv_buffer::buffer_empty(IP_ADDRESS ip, unsigned short port)
{
	pthread_mutex_lock(&_lock);
	IP_PORT ipp = MAKE_UINT64(ip, port);
	auto it = _buffers.find(ipp);
	if (it == _buffers.end())
	{
		pthread_mutex_unlock(&_lock);
		return true;
	}

	bool ret = it->second->empty();
	pthread_mutex_unlock(&_lock);
	return ret;
}

//////////////////////////////////////////////////////////////////////////
void trans_send_buffer::on_arp_resp(IP_ADDRESS ip)
{
	pthread_mutex_lock(&_lock);
	for (auto b : _buffers)
	{
		IP_ADDRESS bip = b.first >> 32;
		if (ip == bip && !b.second->empty())
		{
			pthread_cond_signal(&_no_packets);
			break;
		}
	}
	pthread_mutex_unlock(&_lock);
}

bool trans_send_buffer::get_not_empty_buffer(packet_buffer*& buffer, IP_PORT& ipp)
{
	// 有没有可以处理的包
	for (auto b : _buffers)
	{
		if (!_arp_manager.is_waiting_arp_resp(b.first>>32) && !b.second->empty())
		{
			ipp = b.first;
			buffer = b.second;
			return true;
		}
	}

	return false;
}

bool trans_send_buffer::pop_packet(char* data, int& len, IP_ADDRESS& peer_ip, unsigned short& peer_port)
{
	pthread_mutex_lock(&_lock);

	packet_buffer* buffer = NULL;
	IP_PORT ipp;

	// 没有了，则等有
	if (!get_not_empty_buffer(buffer, ipp))
	{
		pthread_cond_wait(&_no_packets, &_lock);
		get_not_empty_buffer(buffer, ipp);
	}

	peer_ip = (IP_ADDRESS)(ipp >> 32);
	peer_port = (unsigned short)(ipp & 0xFFFFFFFF);
	MAC_ADDRESS dst_mac = _arp_manager.arp_get_cache(peer_ip);
	if (dst_mac == 0)
	{
		arp_send_req(peer_ip);
		pthread_mutex_unlock(&_lock);
		return false;
	}

	bool ret = buffer->pop_packet(data, len);
	pthread_mutex_unlock(&_lock);
	return ret;
}

bool trans_send_buffer::push_packet(const void* data, int len, IP_ADDRESS peer_ip, unsigned short peer_port)
{
	pthread_mutex_lock(&_lock);

	IP_PORT ipp = MAKE_UINT64(peer_ip, peer_port);
	// 如果还没有缓存，则创建
	packet_buffer* pb = NULL;
	auto it = _buffers.find(ipp);
	if (it == _buffers.end())
	{
		pb = new packet_buffer(10 * 1024);
		_buffers[ipp] = pb;
	}
	else
	{
		pb = it->second;
	}

	pb->push_packet(data, len, peer_ip, peer_port);

	if (!_arp_manager.is_waiting_arp_resp(peer_ip))
		pthread_cond_signal(&_no_packets);

	pthread_mutex_unlock(&_lock);
	return true;
}

void trans_send_buffer::try_erase(IP_ADDRESS peer_ip, unsigned short peer_port)
{
	pthread_mutex_lock(&_lock);
	IP_PORT ipp = MAKE_UINT64(peer_ip, peer_port);
	auto it = _buffers.find(ipp);
	if (it != _buffers.end())
	{
		delete it->second;
		_buffers.erase(it);
	}
	pthread_mutex_unlock(&_lock);
}