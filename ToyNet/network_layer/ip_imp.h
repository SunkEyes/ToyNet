#pragma once
#include "../common/common_def.h"

// --------------- Internet Protocol ---------------------------
typedef void(*func_tcp_recv)(char* data, int len, ip_header& header);
typedef void(*func_udp_recv)(char* data, int len, ip_header& header);
struct ip_rx_recver
{
	func_tcp_recv pf_tcp_recv;
	func_udp_recv pf_udp_recv;
};

void ip_init(ip_rx_recver& recver);
int ip_send(const void* data, int len, nps_protocol protocol, IP_ADDRESS dst_ip);
void ip_recv(char* data, int len);
