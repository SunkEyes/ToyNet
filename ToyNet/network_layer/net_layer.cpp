
#include "net_layer.h"

IP_ADDRESS _local_ip;

void net_layer_init(IP_ADDRESS ip)
{
	_local_ip = ip;
}