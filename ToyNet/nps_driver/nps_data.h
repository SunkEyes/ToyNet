#pragma once
#include "common/buffer.h"
#include "common/trans_buffer.h"
#include "common/link_layer_buffer.h"

//////////////////////////////////////////////////////////////////////////
extern data_buffer dll_recv_buffer;	// Э��ջ���ջ��壬��·������д���ݣ���������ȡ����
extern data_buffer dll_send_buffer;	// Э��ջ���ͻ��壬��·�����ȡ���ݣ���������д����

//////////////////////////////////////////////////////////////////////////
extern trans_recv_buffer user_udp_recv_buffer;	// �û��������ݻ��壬Э��ջ����д���ݣ�socket����ȡ����
extern trans_send_buffer user_udp_send_buffer;	// �û��������ݻ��壬socket����д���ݣ�Э��ջ����ȡ����

extern trans_recv_buffer user_tcp_recv_buffer;	// ͬ user_udp_recv_buffer
extern trans_send_buffer user_tcp_send_buffer;	// ͬ user_udp_send_buffer


