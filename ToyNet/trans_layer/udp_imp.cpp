#include "udp_imp.h"
#include "../common/buffer.h"
#include "nps_driver/nps_data.h"

char* udp_unpack(char* data, int& len, udp_header& header)
{
	if (len < udp_header_length)
		return 0;

	memcpy(&header, data, udp_header_length);
	TOY_LOG("->udp", "unpack: port=%d->%d", header.src_port, header.dst_port);

	len -= udp_header_length;
	return data + udp_header_length;
}

void udp_recv(char* data, int len, ip_header& iph)
{
	TOY_LOG("->udp", __FUNCTION__);
	udp_header header;
	char* userdata = udp_unpack(data, len, header);
	user_udp_recv_buffer.push_packet(userdata, len, iph.src_ip, header.src_port, iph.dst_ip, header.dst_port);
}

bool udp_pack(char* data, int& len, void* userdata, int userdatalen, unsigned short from_port, const toy_addr_in* to_addr)
{
	if (userdatalen + udp_header_length > len)
		return false;

	udp_header header;
	header.src_port = from_port;
	header.dst_port = to_addr->port;

	memcpy(data, &header, udp_header_length);
	memcpy(data + udp_header_length, userdata, userdatalen);

	len = userdatalen + udp_header_length;
	return true;
}

int udp_write(void* data, int len, unsigned short local_port, const toy_addr_in* peer)
{
	TOY_LOG("<-udp", "udp_write");
	char packet[MAX_FRAME_BYTES];
	int packet_len = MAX_FRAME_BYTES;
	if (!udp_pack(packet, packet_len, data, len, local_port, peer))
		return -1;

	user_udp_send_buffer.push_packet(packet, packet_len, peer->ip, peer->port);

	return len;
}

int udp_read(char* data, int len, const toy_addr_in* local, toy_addr_in* peer)
{
	TOY_LOG("->udp", "udp_read");
	IP_ADDRESS ip = local->ip;
	unsigned short port = local->port;
	if (!user_udp_recv_buffer.pop_packet(data, len, ip, port))
		return -1;

	peer->ip = ip;
	peer->port = port;
	return len;
}