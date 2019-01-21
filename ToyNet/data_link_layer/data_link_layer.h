#pragma once
#include "../common/common_def.h"

typedef void(*func_ip_recv)(char* data, int len);
typedef void(*func_arp_recv)(char* data, int len, MAC_ADDRESS src_mac);
struct dll_rx_recver
{
	func_ip_recv pf_ipv4_recv;
	func_arp_recv pf_arp_recv;
};

void dll_init(MAC_ADDRESS mac, dll_rx_recver& recver);

int dll_send(const void* data, int len, nps_protocol protocol, MAC_ADDRESS peer_mac);
void dll_recv(char* data, int len);