#pragma once

#include "common/common_def.h"

// --------------- Address Resolution Protocol ---------------------------
void arp_send_req(IP_ADDRESS dst_ip);
void arp_recv(char* data, int len, MAC_ADDRESS src_mac);