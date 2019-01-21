#include "socket_layer/toy_socket.h"
#include "nps_driver/nps_driver.h"
#include <stdio.h>
#include <string.h>
#include "socket_layer/toy_errno.h"
#include <stdlib.h>
#include <conio.h>
#include "common/buffer.h"
#include "pthread.h"

void test_udp(TOY_SOCKET s, toy_addr_in* dst);

// local_mac|local_ip|local_port, dst_ip|dst_port
int main(int argc, const char* argv[])
{
	if (argc < 6)
	{
		printf("������������\n");
		_getch();
		return 0;
	}

	MAC_ADDRESS local_mac = atoi(argv[1]);
	MAC_ADDRESS local_ip = atoi(argv[2]);
	MAC_ADDRESS local_port = atoi(argv[3]);

	MAC_ADDRESS dst_ip = atoi(argv[4]);
	MAC_ADDRESS dst_port = atoi(argv[5]);

	char title[128];
	sprintf_s(title, sizeof(title), "title ����(MAC=%d, IP=%d, �󶨶˿�=%d)", local_mac, local_ip, local_port);
	system(title);

	// Э��ջ����������MAC��IP
	nps_start(local_mac, local_ip);

	TOY_SOCKET s = toy_socket(toy_protocol::TOY_SOCK_DGRAM);
	if (s == TOY_INVALID_SOCKET)
	{
		printf("����socketʧ��!\n");
		return -1;
	}

	toy_addr_in local;
	local.ip = local_ip;
	local.port = local_port;
	toy_bind(s, &local);

	toy_addr_in addr;
	addr.ip = dst_ip;
	addr.port = dst_port;

	test_udp(s, &addr);
	_getch();
	toy_close(s);
	return 0;
}


void print(toy_addr_in peer_addr, char* data, int len)
{
	const char hexes[] = "0123456789abcdef";
	char tmp[MAX_FRAME_BYTES + 1];
	int pos = 0;
	for (int i = 0; i < len; ++i)
	{
		unsigned char a = *(data + i);
		tmp[pos++] = hexes[(int)(a >> 4)];
		tmp[pos++] = hexes[(int)(a & 0x0F)];
	}

	tmp[pos] = 0;
	printf("recv from (%d:%d) %s\n", peer_addr.ip, peer_addr.port, tmp);
}

void to_hex(char* data, int& len)
{
	if (len % 2 != 0)
	{
		len = -1;
		return;
	}

	bool finish = false;
	unsigned char a = 0;
	int pos = 0;
	for (int i = 0; i < len; ++i)
	{
		char c = data[i];
		char cc = 0;
		if (c >= '0' && c <= '9')
			cc = c - '0';
		else if (c >= 'a' && c <= 'f')
			cc = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			cc = c - 'A' + 10;

		if (!finish)
		{
			a += (cc << 4);
			finish = true;
		}
		else
		{
			a += cc;
			data[pos++] = a;
			finish = false;
			a = 0;
		}
	}
	len = pos;
}

void* udp_recv_proc(void* v)
{
	TOY_SOCKET s = (TOY_SOCKET)v;
	while (true)
	{
		char data[MAX_FRAME_BYTES];
		int len = MAX_FRAME_BYTES;
		toy_addr_in peer_addr;
		int read_size = toy_recvfrom(s, data, len, &peer_addr);
		if (read_size > 0)
			print(peer_addr, data, read_size);
	}

	return NULL;
}

void test_udp(TOY_SOCKET s, toy_addr_in* dst)
{
	pthread_t recv_thread;
	pthread_create(&recv_thread, 0, udp_recv_proc, (void*)s);

	printf("�����������һ������:\n  q=�˳�\n  a=�༭����һ���ı�����\n  b=�༭����һ������������(ʮ�������ı���ʽ)\n");
	while (true)
	{
		char data[MAX_FRAME_BYTES];
		char c = _getch();
		if (c == 'q')
			break;
		else if (c == 'a')
		{
			printf("�������ı�:");
			gets_s(data);
			int len = strlen(data);
			if (len == 0)
			{
				printf("���ݲ��Ϸ�\n");
				continue;
			}

			printf("����һ������: %s\n", data);
		}
		else if (c == 'b')
		{
			printf("������ʮ����������:");
			gets_s(data);
			printf("����һ������: 0x %s\n", data);
			int len = strlen(data);
			to_hex(data, len);
			if (len == 0 || len == -1)
			{
				printf("���ݲ��Ϸ�\n");
				continue;
			}
			data[len] = 0;
		}
		else
		{
			strcpy_s(data, sizeof(data), "12345678");
			printf("����һ������: %s\n", data);
		}
		toy_sendto(s, data, strlen(data), dst);
	}

	// exit
	
}