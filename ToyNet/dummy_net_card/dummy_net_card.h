#include <winsock2.h>
#include "../common/common_def.h"
#include "pthread.h"

// 数据有边界的网卡，真高级[滑稽]
class dummy_net_card
{
public:
	MAC_ADDRESS _mac;
	SOCKET _net_cable;		// 模拟网线

public:
	bool open_device(MAC_ADDRESS mac);

private:
	pthread_t hRecv, hSend;
	bool recv_proc_exit;
	bool send_proc_exit;
	static void* recv_proc(void* v);
	static void* send_proc(void* v);

public:
	// 连上网线
	void cable_enable();
	// 拔下网线
	void cable_disable();

public:
	enum class data_type
	{
		DATA = 0,
		ENABLE = 1,
		DISABLE = 2,
	};
	// 网卡只负责收发
	void send(data_type type = data_type::DATA);
	void recv();
};