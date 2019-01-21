#include <stdio.h>  
#include <winsock2.h>  
#include <map>
#include <vector>
#include "../common/common_def.h"

#pragma comment(lib,"ws2_32.lib")

// �����б������б�Ҳ�������б�
std::map<UINT64, sockaddr_in> netcardList;

const char* device_name(UINT64 id)
{
	static char name[32] = { 0 };
	int ip = id >> 32;
	unsigned short port = id & 0xffff;

	in_addr addr;
	addr.S_un.S_addr = ip;
	sprintf(name, "%s:%d", inet_ntoa(addr), ntohs(port));
	return name;
}

// ��װ����һ��10000�ڼ�����+��ͨ�˴������
int main(int argc, char* argv[])
{
	UINT64 totalBytes = 0;
	int lastShow = GetTickCount();
	
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("bind error !");
		closesocket(serSocket);
		return 0;
	}

	printf("10000�ڼ�����׼������...\n");

	sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);
	while (true)
	{
		char recvData[1024];
		char consoleTitle[64];
		int recvLen = recvfrom(serSocket, recvData, 1024, 0, (sockaddr *)&remoteAddr, &nAddrLen);
		UINT64 id = MAKE_UINT64(remoteAddr.sin_addr.S_un.S_addr, remoteAddr.sin_port);
		// -1,����Ч����
		if (recvLen == -1)
		{
			printf("[!!]�豸����: %s\n", device_name(id));
			netcardList.erase(id);
		}		
		// һ���ַ���Ӧ���ǿ���ָ��
		else if (recvLen == 1)
		{
			switch ((int)recvData[0])
			{
			case 1:
				if (netcardList.find(id) == netcardList.end())
				{
					printf("[+]��������: %s\n", device_name(id));
					netcardList[id] = remoteAddr;
				}
				break;
			case 2:
				if (netcardList.find(id) != netcardList.end())
				{
					printf("[-]��������: %s\n", device_name(id));
					netcardList.erase(id);
				}
				break;
			}
		}
		// ����������
		else
		{
			if (recvLen > 0)
			{
				totalBytes += recvLen;
				if (GetTickCount() - lastShow > 2000)
				{
					lastShow = GetTickCount();
					sprintf(consoleTitle, "title [��ڼ�����]����:[ %llu ]", totalBytes);
					system(consoleTitle);
				}

				for (auto& card : netcardList)
				{
					if (card.first == id)
						continue;
					int ret = sendto(serSocket, &recvData[1], recvLen-1, 0, (sockaddr *)&card.second, sizeof(card.second));
				}
			}
		}
	}
	closesocket(serSocket);
	WSACleanup();
	return 0;
}