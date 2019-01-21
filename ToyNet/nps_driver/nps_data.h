#pragma once
#include "common/buffer.h"
#include "common/trans_buffer.h"
#include "common/link_layer_buffer.h"

//////////////////////////////////////////////////////////////////////////
extern data_buffer dll_recv_buffer;	// 协议栈接收缓冲，链路层往里写数据，网卡从里取数据
extern data_buffer dll_send_buffer;	// 协议栈发送缓冲，链路层从里取数据，网卡往里写数据

//////////////////////////////////////////////////////////////////////////
extern trans_recv_buffer user_udp_recv_buffer;	// 用户接收数据缓冲，协议栈往里写数据，socket从里取数据
extern trans_send_buffer user_udp_send_buffer;	// 用户发送数据缓冲，socket往里写数据，协议栈从里取数据

extern trans_recv_buffer user_tcp_recv_buffer;	// 同 user_udp_recv_buffer
extern trans_send_buffer user_tcp_send_buffer;	// 同 user_udp_send_buffer


