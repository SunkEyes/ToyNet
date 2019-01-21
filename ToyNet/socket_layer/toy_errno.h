#pragma once

extern int toy_errno;

enum class toy_error
{
	success = 0,


	// 10000+ socket错误
	SOCKET_EXCEED = 10001,			// socket数量超出上限了
	SOCKET_INVALID = 10002,			// 调用的函数与协议不匹配
	SOCKET_INVALID_PROTO = 10003,	// 调用的函数与协议不匹配
	PORT_IN_USE = 10004,			// 端口被占用
	SOCKET_NEED_BIND = 10005,		// 要绑定端口才能使用
};

void set_toy_errno(toy_error err);