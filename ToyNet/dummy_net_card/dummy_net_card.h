#include <winsock2.h>
#include "../common/common_def.h"
#include "pthread.h"

// �����б߽����������߼�[����]
class dummy_net_card
{
public:
	MAC_ADDRESS _mac;
	SOCKET _net_cable;		// ģ������

public:
	bool open_device(MAC_ADDRESS mac);

private:
	pthread_t hRecv, hSend;
	bool recv_proc_exit;
	bool send_proc_exit;
	static void* recv_proc(void* v);
	static void* send_proc(void* v);

public:
	// ��������
	void cable_enable();
	// ��������
	void cable_disable();

public:
	enum class data_type
	{
		DATA = 0,
		ENABLE = 1,
		DISABLE = 2,
	};
	// ����ֻ�����շ�
	void send(data_type type = data_type::DATA);
	void recv();
};