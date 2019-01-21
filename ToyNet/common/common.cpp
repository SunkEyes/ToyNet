
#include <stdio.h>
#include "common.h"
#include "common_def.h"

//////////////////////////////////////////////////////////////////////////
const char* _nps_protocol_names[(int)nps_protocol::max] = { "" };
struct nps_protocol_names_init
{
	nps_protocol_names_init()
	{
		_nps_protocol_names[(int)nps_protocol::ipv4] = "ipv4";
		_nps_protocol_names[(int)nps_protocol::arp] = "arp";

		_nps_protocol_names[(int)nps_protocol::tcp] = "tcp";
		_nps_protocol_names[(int)nps_protocol::udp] = "udp";
	}
} _nps_protocol_names_init;


const char* get_nps_protocol_name(BYTE protocol)
{
	return _nps_protocol_names[protocol];
}


// bool split_config(const char* file, std::map<std::string, std::string>& kv)
// {
// 	FILE* fp = fopen(file, "rt");
// 	if (fp == NULL)
// 		return false;
// 
// 	char line[128];
// 	int bytecnt = 0;
// 	while (fgets(line, 128, fp))
// 	{
// 		if (strstr(line, "\n") == NULL)
// 		{
// 			if (fgets(line, 128, fp) != NULL)
// 				return false;
// 		}
// 
// 		char* k = strtok(line, "=");
// 		char *v = strtok(NULL, "\n");
// 
// 		kv[k] = v;
// 		printf("%s = %s\n", k, v);
// 	}
// 
// 	return true;
// }