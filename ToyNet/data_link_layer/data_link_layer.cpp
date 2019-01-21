#include "data_link_layer.h"
#include "../common/buffer.h"
#include "../dummy_net_card/dummy_net_card.h"
#include "nps_driver/nps_data.h"

dummy_net_card net_card;	// 链路层绑定一个网卡
dll_rx_recver _recver;

void dll_init(MAC_ADDRESS mac, dll_rx_recver& recver)
{
	_recver = recver;
	net_card.open_device(mac);
	net_card.cable_enable();
}

bool dll_pack(const void* data, int len, char* packet, int& packlen, nps_protocol protocol, MAC_ADDRESS peer_mac)
{
	if (len + dll_header_length > packlen)
		return false;

	dll_header dh;
	dh.protocol = (BYTE)protocol;
	dh.dst_mac = peer_mac;
	dh.src_mac = net_card._mac;

	memcpy(packet, &dh, dll_header_length);
	memcpy(packet + dll_header_length, data, len);

	packlen = len + dll_header_length;

	return true;
}

char* dll_unpack(char* data, int& len, dll_header& header)
{
	memcpy(&header, data, dll_header_length);
	TOY_LOG("->dll", "unpack: mac=%d->%d %s", header.src_mac, header.dst_mac, get_nps_protocol_name(header.protocol));
	if (header.protocol != (BYTE)nps_protocol::arp)
	{
		// 非ARP协议，那么目的MAC不是自己的包丢弃
		if (header.dst_mac != net_card._mac)
			return 0;
	}

	len -= dll_header_length;
	return data + dll_header_length;
}

int dll_send(const void* data, int len, nps_protocol protocol, MAC_ADDRESS peer_mac)
{
	TOY_LOG("<-dll", "dll_send");

	char packet[MAX_FRAME_BYTES];
	int packlen = MAX_FRAME_BYTES;
	if (!dll_pack(data, len, packet, packlen, protocol, peer_mac))
		return -1;

	return dll_send_buffer.push_packet(packet, packlen);
}

void dll_recv(char* data, int len)
{
	TOY_LOG("->dll", "dll_recv");
	dll_header header;
	char* net_packet = dll_unpack(data, len, header);
	if (net_packet == NULL)
	{
		// 不是网络层的包，或者不是本网卡的包
		return;
	}

	switch (header.protocol)
	{
	case (BYTE)nps_protocol::ipv4:
		_recver.pf_ipv4_recv(net_packet, len);
		break;

	case (BYTE)nps_protocol::arp:
		_recver.pf_arp_recv(net_packet, len, header.src_mac);
		break;
	}
}