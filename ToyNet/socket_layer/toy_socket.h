#pragma once

#define TOY_SOCKET int
#define TOY_INVALID_SOCKET (-1)

typedef int IP_ADDRESS;
typedef int MAC_ADDRESS;

enum class toy_protocol
{
	TOY_SOCK_UNKNOWN = 0,
	TOY_SOCK_STREAM = 1,	// tcp
	TOY_SOCK_DGRAM = 2,		// udp
	TOY_SOCK_RAW = 3		// ip
};

struct toy_addr_in
{
	IP_ADDRESS ip;
	unsigned short port;
};

TOY_SOCKET toy_socket(toy_protocol type);
void toy_close(TOY_SOCKET s);
int toy_bind(TOY_SOCKET s, toy_addr_in* addr);
int toy_sendto(TOY_SOCKET s, void* data, int len, toy_addr_in* addr);
int toy_recvfrom(TOY_SOCKET s, char* data, int len, toy_addr_in* addr);
