#include "arp_cache.h"
#include "nps_driver\nps_data.h"
#include "network_layer\arp_imp.h"

//////////////////////////////////////////////////////////////////////////
arp_manager _arp_manager;


//////////////////////////////////////////////////////////////////////////

arp_manager::arp_manager()
{
	pthread_mutex_init(&_lock_cache, NULL);

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&_lock_waiting_resp, &attr);
}

MAC_ADDRESS arp_manager::arp_get_cache(IP_ADDRESS ip)
{
	pthread_mutex_lock(&_lock_cache);
	MAC_ADDRESS mac = 0;
	auto it = _arp_cache.find(ip);
	if (it != _arp_cache.end())
		mac = it->second;
	pthread_mutex_unlock(&_lock_cache);

	return mac;
}

void arp_manager::arp_set_cache(IP_ADDRESS ip, MAC_ADDRESS mac)
{
	pthread_mutex_lock(&_lock_waiting_resp);
	auto it = _arp_waiting_resp.find(ip);
	bool not_wating_resp = true;
	if (it != _arp_waiting_resp.end())
	{
		_arp_waiting_resp.erase(it);
		not_wating_resp = false;
	}
	pthread_mutex_unlock(&_lock_waiting_resp);

	if (not_wating_resp)
		return;

	pthread_mutex_lock(&_lock_cache);
	_arp_cache[ip] = mac;
	pthread_mutex_unlock(&_lock_cache);

	user_udp_send_buffer.on_arp_resp(ip);
}

//
void arp_manager::on_send_arp_to(IP_ADDRESS ip)
{
	pthread_mutex_lock(&_lock_waiting_resp);
	_arp_waiting_resp[ip] = time(NULL) + 5;
	pthread_mutex_unlock(&_lock_waiting_resp);
}

bool arp_manager::is_waiting_arp_resp(IP_ADDRESS ip)
{
	pthread_mutex_lock(&_lock_waiting_resp);
	bool iswaiting = false;
	auto it = _arp_waiting_resp.find(ip);
	if (it != _arp_waiting_resp.end())
		iswaiting = true;
	pthread_mutex_unlock(&_lock_waiting_resp);

	return iswaiting;
}

void arp_manager::arp_timeout_resend(time_t now)
{
	pthread_mutex_lock(&_lock_waiting_resp);
	for (auto it = _arp_waiting_resp.begin(); it != _arp_waiting_resp.end(); ++it)
	{
		if (now > it->second)
		{
			// 这个函数调用on_send_arp_to
			arp_send_req(it->first);
		}
	}
	pthread_mutex_unlock(&_lock_waiting_resp);
}