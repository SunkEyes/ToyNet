#include "nps_driver.h"
#include "pthread.h"
#include "../data_link_layer/data_link_layer.h"
#include "../network_layer/net_layer.h"
#include "../common/buffer.h"
#include "../trans_layer/udp_imp.h"
#include "network_layer/ip_imp.h"
#include "network_layer/arp_imp.h"
#include "nps_data.h"
#include "common/arp_cache.h"

void* nps_recv_proc(void *v)
{
	while (true)
	{
		// ���������ݣ�û����ȴ�
		char packet[MAX_FRAME_BYTES];
		int len = MAX_FRAME_BYTES;
		dll_recv_buffer.pop_packet(packet, len);

		// ����ϵ�
		dll_recv(packet, len);
	}

	return NULL;
}

void* nps_send_proc(void *v)
{
	while (true)
	{
		// ���������ݣ�û����ȴ�
		char packet[MAX_FRAME_BYTES];
		int len = MAX_FRAME_BYTES;
		
		IP_ADDRESS ip;
		unsigned short port;
		if (user_udp_send_buffer.pop_packet(packet, len, ip, port))
		{
			// ����·�
			ip_send(packet, len, nps_protocol::udp, ip);
		}
	}

	return NULL;
}


// �����̣߳����г�ʱ�ж���������
void* nps_control_proc(void *v)
{
	pthread_cond_t time_interval;
	pthread_mutex_t lock;
	pthread_mutex_init(&lock, 0);
	pthread_cond_init(&time_interval, 0);

	while (true)
	{
		time_t now = time(NULL);
		//printf("now=%d ", now);

		// ִ��һ���ж�
		// arp��ʱ�ж�
		_arp_manager.arp_timeout_resend(now);

		// ��1����
		// ��ֹ��1���time(NULL)��ֵû�б�
		timespec ts;
		ts.tv_nsec = 0;
		ts.tv_sec = now + 1;
		pthread_mutex_lock(&lock);
		pthread_cond_timedwait(&time_interval, &lock, &ts);
		pthread_mutex_unlock(&lock);
	}
}

bool nps_start(MAC_ADDRESS mac, IP_ADDRESS ip)
{
	net_layer_init(ip);

	ip_rx_recver iprx;
	iprx.pf_udp_recv = udp_recv;
	iprx.pf_tcp_recv = NULL;
	ip_init(iprx);

	dll_rx_recver dllrx;
	dllrx.pf_ipv4_recv = ip_recv;
	dllrx.pf_arp_recv = arp_recv;
	dll_init(mac, dllrx);

	pthread_t recv_thread;
	pthread_t send_thread;
	pthread_t control_thread;
	pthread_create(&recv_thread, NULL, nps_recv_proc, NULL);
	pthread_create(&send_thread, NULL, nps_send_proc, NULL);
	pthread_create(&control_thread, NULL, nps_control_proc, NULL);

	return true;
}