#pragma once
#include <map>
#include "pthread.h"
#include "buffer.h"

struct trans_recv_buffer
{
	std::map<IP_PORT, packet_buffer*> _buffers;
	pthread_mutex_t _lock;
	pthread_cond_t _exist_packets;

	trans_recv_buffer()
	{
		pthread_mutex_init(&_lock, 0);
		pthread_cond_init(&_exist_packets, 0);
	}
	~trans_recv_buffer(){}

	bool pop_packet(char* data, int& len, IP_ADDRESS& peer_ip, unsigned short& peer_port);
	bool push_packet(const void* data, int len, IP_ADDRESS peer_ip, unsigned short peer_port, IP_ADDRESS local_ip, unsigned short local_port);

	bool try_erase(IP_ADDRESS ip, unsigned short port);
	bool has(IP_ADDRESS ip, unsigned short port);
	void add(IP_ADDRESS ip, unsigned short port, int buff_size);
	bool buffer_empty(IP_ADDRESS ip, unsigned short port);
};


struct trans_send_buffer
{
	std::map<IP_PORT, packet_buffer*> _buffers;
	pthread_mutex_t _lock;
	pthread_cond_t _no_packets;

	trans_send_buffer()
	{
		pthread_mutex_init(&_lock, 0);
		pthread_cond_init(&_no_packets, 0);
	}
	~trans_send_buffer() {}

	bool pop_packet(char* data, int& len, IP_ADDRESS& peer_ip, unsigned short& peer_port);
	bool push_packet(const void* data, int len, IP_ADDRESS peer_ip, unsigned short peer_port);
	void try_erase(IP_ADDRESS peer_ip, unsigned short peer_port);

	void on_arp_resp(IP_ADDRESS ip);

private:
	bool get_not_empty_buffer(packet_buffer*& buffer, IP_PORT& ipp);
};
