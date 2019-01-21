
#include "../dummy_net_card/dummy_net_card.h"
#include "../common/common.h"
#include "../common/buffer.h"
#include "conio.h"
#include "../nps_driver/nps_driver.h"
#include "../socket_layer/toy_socket.h"
#include "../nps_driver/nps_data.h"

void test_net_card();

int main()
{
	test_net_card();

	//test2();

	return 0;
}

void print(char* data, int len)
{
	const char hexes[] = "0123456789abcdef";
	char tmp[MAX_FRAME_BYTES + 1];
	int pos = 0;
	for (int i = 0; i < len; ++i)
	{
		unsigned char a = *(data + i);
		tmp[pos++] = hexes[(int)(a >> 4)];
		tmp[pos++] = hexes[(int)(a & 0x0F)];
	}

	tmp[pos] = 0;
	printf("recv: %s\n", tmp);
}

void to_hex(char* data, int& len)
{
	if (len % 2 != 0)
	{
		len = -1;
		return;
	}

	bool finish = false;
	unsigned char a = 0;
	int pos = 0;
	for (int i = 0; i < len; ++i)
	{
		char c = data[i];
		char cc = 0;
		if (c >= '0' && c <= '9')
			cc = c - '0';
		else if (c >= 'a' && c <= 'f')
			cc = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			cc = c - 'A' + 10;

		if (!finish)
		{
			a += (cc << 4);
			finish = true;
		}
		else
		{
			a += cc;
			data[pos++] = a;
			finish = false;
			a = 0;
		}
	}
	len = pos;
}

void test_net_card()
{
	dummy_net_card netcard;
	netcard.open_device(10001);

	netcard.cable_enable();

	printf("�����������һ������:\n  q=�˳�\n  a=�༭����һ���ı�����\n  b=�༭����һ������������(ʮ�������ı���ʽ)\n");
	while(true)
	{
		int key = 0;
		if (key = kbhit())
		{
			char data[MAX_FRAME_BYTES];
			char c = getch();
			if (c == 'q')
				break;
			else if (c == 'a')
			{
				printf("�������ı�:");
				gets_s(data);
				int len = strlen(data);
				if (len == 0)
				{
					printf("���ݲ��Ϸ�\n");
					continue;
				}

				printf("����һ������: %s\n", data);
			}
			else if (c == 'b')
			{
				printf("������ʮ����������:");
				gets_s(data);
				printf("����һ������: 0x %s\n", data);
				int len = strlen(data);
				to_hex(data, len);
				if (len == 0 || len == -1)
				{
					printf("���ݲ��Ϸ�\n");
					continue;
				}
				data[len] = 0;
			}
			else
			{
				strcpy(data, "12345678");
				printf("����һ������: %s\n", data);
			}
			dll_send_buffer.push_packet(data, strlen(data));
		}
		else
		{
			if (!dll_recv_buffer.empty())
			{
				char data[MAX_FRAME_BYTES];
				int len = MAX_FRAME_BYTES;
				dll_recv_buffer.pop_packet(data, len);
				print(data, len);
			}
		}
	}

	netcard.cable_disable();
}