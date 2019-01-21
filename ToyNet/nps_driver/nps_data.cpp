#include "nps_data.h"

// 链路层
data_buffer dll_recv_buffer(10 * 1024);
data_buffer dll_send_buffer(10 * 1024);

trans_recv_buffer user_udp_recv_buffer;
trans_send_buffer user_udp_send_buffer;

trans_recv_buffer user_tcp_recv_buffer;
trans_send_buffer user_tcp_send_buffer;

std::map<IP_ADDRESS, int> _arp_reqs;		// arp请求，等待回应的