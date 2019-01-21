
#include "ip_imp.h"
#include "common/common_def.h"
#include <memory.h>
#include "data_link_layer/data_link_layer.h"
#include "common/buffer.h"
#include "common/arp_cache.h"

ip_rx_recver _ip_rx_recver;

void ip_init(ip_rx_recver& recver)
{
	_ip_rx_recver = recver;
}

bool ip_pack(const void* data, int len, char* packet, int& packlen, nps_protocol protocol, IP_ADDRESS dst_ip)
{
	if (len + ip_header_length > packlen)
		return false;

	ip_header h;
	h.protocol = (BYTE)protocol;
	h.ttl = 10;
	h.src_ip = _local_ip;
	h.dst_ip = dst_ip;

	memcpy(packet, &h, ip_header_length);
	memcpy(packet + ip_header_length, data, len);
	packlen = len + ip_header_length;
	return true;
}

char* ip_unpack(char* data, int& len, ip_header& header)
{
	if (len < ip_header_length)
		return 0;

	memcpy(&header, data, ip_header_length);
	TOY_LOG("->ip", "unpack: ip=%d->%d %s", header.src_ip, header.dst_ip, get_nps_protocol_name(header.protocol));

	len -= ip_header_length;
	return data + ip_header_length;
}

int ip_send(const void* data, int len, nps_protocol protocol, IP_ADDRESS dst_ip)
{
	TOY_LOG("<-ip", "ip_send");
	char packet[MAX_FRAME_BYTES];
	int packet_len = MAX_FRAME_BYTES;
	if (!ip_pack(data, len, packet, packet_len, protocol, dst_ip))
		return -1;

	MAC_ADDRESS peer_mac = _arp_manager.arp_get_cache(dst_ip);
	return dll_send(packet, packet_len, nps_protocol::ipv4, peer_mac);
}

void ip_recv(char* data, int len)
{
	TOY_LOG("->ip", "ip_recv");
	ip_header header;
	char* trans_packet = ip_unpack(data, len, header);

	switch (header.protocol)
	{
	case (BYTE)nps_protocol::tcp:
		_ip_rx_recver.pf_tcp_recv(trans_packet, len, header);
		break;
		
	case (BYTE)nps_protocol::udp:
		_ip_rx_recver.pf_udp_recv(trans_packet, len, header);
		break;
	}
}