#pragma once
#include "../common/common_def.h"
#include "../socket_layer/toy_socket.h"

void udp_recv(char* data, int len, ip_header& iph);

int udp_read(char* data, int len, const toy_addr_in* local, toy_addr_in* peer);
int udp_write(void* data, int len, unsigned short localport, const toy_addr_in* peer);