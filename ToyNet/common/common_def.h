#pragma once

#ifdef _WIN32
#include <basetsd.h>
#endif

#include <stdio.h>
#define TOY_LOG(mod, msg, ...) printf("[%-5s]  " msg "\n", mod, ##__VA_ARGS__)

typedef int MAC_ADDRESS;
typedef int IP_ADDRESS;
typedef unsigned char BYTE;
typedef UINT64 IP_PORT;

#define SYS_ROOT "../ToyNet/sys_root"
#define MAX_FRAME_BYTES 1000

#define MAKE_UINT64(ip,port) ((((UINT64)ip)<<32)|(UINT64)port)

#pragma pack(push,1)
struct tcp_header
{
	unsigned short src_port;
	unsigned short dst_port;
};
static const int tcp_header_length = sizeof(tcp_header);

struct udp_header
{
	unsigned short src_port;
	unsigned short dst_port;
};
static const int udp_header_length = sizeof(udp_header);

struct ip_header
{
	IP_ADDRESS src_ip;
	IP_ADDRESS dst_ip;
	BYTE ttl;
	BYTE protocol;	// 传输层协议号
};
static const int ip_header_length = sizeof(ip_header);

struct arp_header
{
	IP_ADDRESS src_ip;
	IP_ADDRESS dst_ip;
	BYTE ttl;
	BYTE req;		// 1=arp请求， 2=arp应答
};
static const int arp_header_length = sizeof(arp_header);

struct dll_header
{
	MAC_ADDRESS src_mac;
	MAC_ADDRESS dst_mac;
	BYTE protocol;		// 网络层协议号
};
static const int dll_header_length = sizeof(dll_header);
#pragma pack(pop)

// 一个byte
enum class nps_protocol
{
	// 11-29 留给网络层
	ipv4 = 11,
	arp = 12,

	// 31-49 留给传输层
	tcp = 31,
	udp = 32,

	max = 100
};

const char* get_nps_protocol_name(BYTE protocol);