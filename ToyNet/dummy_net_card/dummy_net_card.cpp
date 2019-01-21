
#include "dummy_net_card.h"
#include "../common/common.h"
#include "../common/buffer.h"
#include "nps_driver/nps_data.h"

bool dummy_net_card::open_device(MAC_ADDRESS mac)
{
	_mac = mac;
	return true;
}

void* dummy_net_card::send_proc(void* v)
{
	auto card = (dummy_net_card*)v;

	while (!card->send_proc_exit)
		card->send();

	return NULL;
}

void* dummy_net_card::recv_proc(void* v)
{
	auto card = (dummy_net_card*)v;

	while(!card->recv_proc_exit)
		card->recv();

	return NULL;
}

void dummy_net_card::send(data_type type)
{
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int slen = sizeof(sin);

	if (type == data_type::DATA)
	{
		char data[MAX_FRAME_BYTES+1];
		data[0] = (char)type;
		int len = MAX_FRAME_BYTES;
		dll_send_buffer.pop_packet(&data[1], len);
		sendto(_net_cable, (const char*)data, len + 1, 0, (sockaddr*)&sin, slen);
	}
	else
	{
		char cmd = (char)type;
		sendto(_net_cable, &cmd, 1, 0, (sockaddr*)&sin, slen);
	}
}

void dummy_net_card::recv()
{
	char data[MAX_FRAME_BYTES];
	int len = MAX_FRAME_BYTES;
	len = recvfrom(_net_cable, data, len, 0, NULL, NULL);
	if (len > 0)
	{
		dll_recv_buffer.push_packet(data, len);
	}
	else
	{
		int err = GetLastError();
		printf("-%d", err);
	}
}

void dummy_net_card::cable_enable()
{
	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(socketVersion, &wsaData);

	_net_cable = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//int imode = 1;
	//int ret = ioctlsocket(_net_cable, FIONBIO, (u_long *)&imode);

	send( data_type::ENABLE);

	recv_proc_exit = false;
	send_proc_exit = false;

	pthread_create(&hRecv, 0, recv_proc, this);
	pthread_create(&hSend, 0, send_proc, this);
}

void dummy_net_card::cable_disable()
{
	send_proc_exit = true;
	recv_proc_exit = true;
	send(data_type::DISABLE);
	// 粗暴断开，不需等待
	//pthread_join(hSend, NULL);
	//pthread_join(hRecv, NULL);
	
	closesocket(_net_cable);
	WSACleanup();
}