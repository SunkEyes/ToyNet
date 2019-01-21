#pragma once
#include "common_def.h"
#include <map>
#include "pthread.h"

// ARP缓存、请求管理
struct arp_manager
{
public:
	arp_manager();

	MAC_ADDRESS arp_get_cache(IP_ADDRESS ip);
	void arp_set_cache(IP_ADDRESS ip, MAC_ADDRESS mac);

public:
	void on_send_arp_to(IP_ADDRESS ip);
	bool is_waiting_arp_resp(IP_ADDRESS ip);
	void arp_timeout_resend(time_t now);

private:
	pthread_mutex_t _lock_cache;
	std::map<IP_ADDRESS, MAC_ADDRESS> _arp_cache;		// arp缓存

	pthread_mutex_t _lock_waiting_resp;
	std::map<IP_ADDRESS, time_t> _arp_waiting_resp;		// arp请求待回应列表
};

extern arp_manager _arp_manager;