
#include "toy_socket.h"
#include <vector>
#include "../common/common_def.h"
#include "../common/buffer.h"
#include "../trans_layer/udp_imp.h"
#include "toy_errno.h"
#include "nps_driver/nps_data.h"

#define MAX_SOCK_NUM 100

struct socket_info
{
	char used;
	toy_protocol proto;
	toy_addr_in local;
	toy_addr_in peer;

	socket_info()
	{
		used = 0;
		proto = toy_protocol::TOY_SOCK_UNKNOWN;
		local.ip = 0;
		local.port = 0;
	}

	void clear()
	{
		used = 0;
		proto = toy_protocol::TOY_SOCK_UNKNOWN;

		// 已经绑定了对端(发送消息导致的)
		if (peer.port > 0)
		{
			user_udp_send_buffer.try_erase(peer.ip, peer.port);
		}

		// 已经绑定了本地端
		if (local.port > 0)
		{
			user_udp_recv_buffer.try_erase(local.ip, local.port);
		}

		local.port = 0;
		peer.port = 0;
	}
};

struct SocketData
{
	socket_info sockList[MAX_SOCK_NUM];
};


SocketData _socketData;


TOY_SOCKET toy_socket(toy_protocol type)
{
	for (int i = 0; i < MAX_SOCK_NUM; ++i)
	{
		if (_socketData.sockList[i].used == 0)
		{
			_socketData.sockList[i].used = 1;
			_socketData.sockList[i].proto = type;
			return i;
		}
	}

	set_toy_errno(toy_error::SOCKET_EXCEED);
	return -1;
}

void toy_close(TOY_SOCKET s)
{
	if (s < 0 || s >= MAX_SOCK_NUM)
		return;

	_socketData.sockList[(int)s].clear();
}

int toy_bind(TOY_SOCKET s, toy_addr_in* addr)
{
	if (s < 0 || s >= MAX_SOCK_NUM)
	{
		set_toy_errno(toy_error::SOCKET_EXCEED);
		return -1;
	}

	socket_info* sock = &_socketData.sockList[(int)s];
	if (sock->used == 0)
	{
		set_toy_errno(toy_error::SOCKET_INVALID);
		return -1;
	}

	sock->local = *addr;
	
	// 协议栈只有一个时，才能保证这个被占用判断准确
	if (user_udp_recv_buffer.has(addr->ip, addr->port))
	{
		set_toy_errno(toy_error::PORT_IN_USE);
		return -1;
	}

	user_udp_recv_buffer.add(addr->ip, addr->port, 10 * 1024);
	return 0;
}

int toy_sendto(TOY_SOCKET s, void* data, int len, toy_addr_in* peer)
{
	if (s < 0 || s >= MAX_SOCK_NUM)
	{
		set_toy_errno(toy_error::SOCKET_EXCEED);
		return -1;
	}

	socket_info* sock = &_socketData.sockList[(int)s];
	if (sock->proto != toy_protocol::TOY_SOCK_DGRAM)
	{
		set_toy_errno(toy_error::SOCKET_INVALID_PROTO);
		return -1;
	}

	if (sock->local.port == 0)
	{
		set_toy_errno(toy_error::SOCKET_NEED_BIND);
		// 暂时不做本地端口自动分配
	//	if (toy_bind(s, to) != 0)
			return -1;
	}

	return udp_write(data, len, sock->local.port, peer);
}

int toy_recvfrom(TOY_SOCKET s, char* data, int len, toy_addr_in* peer)
{
	if (s < 0 || s >= MAX_SOCK_NUM)
	{
		set_toy_errno(toy_error::SOCKET_EXCEED);
		return -1;
	}

	socket_info* sock = &_socketData.sockList[(int)s];
	if (sock->proto != toy_protocol::TOY_SOCK_DGRAM)
	{
		set_toy_errno(toy_error::SOCKET_INVALID_PROTO);
		return -1;
	}

	int rs = udp_read(data, len, &sock->local, peer);
	return rs;
}