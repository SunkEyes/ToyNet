#pragma once

extern int toy_errno;

enum class toy_error
{
	success = 0,


	// 10000+ socket����
	SOCKET_EXCEED = 10001,			// socket��������������
	SOCKET_INVALID = 10002,			// ���õĺ�����Э�鲻ƥ��
	SOCKET_INVALID_PROTO = 10003,	// ���õĺ�����Э�鲻ƥ��
	PORT_IN_USE = 10004,			// �˿ڱ�ռ��
	SOCKET_NEED_BIND = 10005,		// Ҫ�󶨶˿ڲ���ʹ��
};

void set_toy_errno(toy_error err);