#include "arp_imp.h"
#include <memory.h>
#include "common/arp_cache.h"
#include "data_link_layer/data_link_layer.h"
#include "net_layer.h"

//////////////////////////////////////////////////////////////////////////
bool arp_pack(char* packet, int& packlen, IP_ADDRESS dst_ip, BYTE arp_type)
{
	if (arp_header_length > packlen)
		return false;

	arp_header h;
	h.src_ip = _local_ip;
	h.dst_ip = dst_ip;
	h.ttl = 10;
	h.req = arp_type;

	memcpy(packet, &h, arp_header_length);
	packlen = arp_header_length;
	return true;
}

char* arp_unpack(char* data, int& len, arp_header& header)
{
	if (len < arp_header_length)
		return 0;

	memcpy(&header, data, arp_header_length);

	len -= arp_header_length;
	return data + arp_header_length;
}

// arp请求
void arp_send_req(IP_ADDRESS dst_ip)
{
	TOY_LOG("<-arp", "arp_send");
	_arp_manager.on_send_arp_to(dst_ip);

	char packet[MAX_FRAME_BYTES];
	int packlen = MAX_FRAME_BYTES;
	arp_pack(packet, packlen, dst_ip, 1);
	dll_send(packet, packlen, nps_protocol::arp, 0/*请求对方的MAC地址，这里填0*/);
}

// arp应答
void arp_send_resp(IP_ADDRESS peer_ip, MAC_ADDRESS peer_mac)
{
	TOY_LOG("<-arp", "arp_send");
	char packet[MAX_FRAME_BYTES];
	int packlen = MAX_FRAME_BYTES;
	arp_pack(packet, packlen, peer_ip, 2);

	dll_send(packet, packlen, nps_protocol::arp, peer_mac);
}

void arp_recv(char* data, int len, MAC_ADDRESS src_mac)
{
	arp_header ah;
	if (!arp_unpack(data, len, ah))
		return;

	if (ah.dst_ip != _local_ip)
		return;

	TOY_LOG("->arp", "arp_recv:%s from mac=%d ip=%d", (ah.req==1?"请求":"应答"), src_mac, ah.src_ip);
	switch (ah.req)
	{
	case 1:	// arp请求
		arp_send_resp(ah.src_ip, src_mac);	// 回复我的MAC地址
		break;

	case 2:	// arp应答
		_arp_manager.arp_set_cache(ah.src_ip, src_mac);
		break;

	default:
		return;
	}
}
