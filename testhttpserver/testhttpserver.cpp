// testhttpserver.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <memory>
#include "src\http_server.h"


bool handle_fun1(std::string url, std::string body, mg_connection* c, OnRspCallback rsp_callback)
{
	// do sth
	std::cout << "handle fun1" << std::endl;
	std::cout << "url: " << url << std::endl;
	std::cout << "body: " << body << std::endl;

	rsp_callback(c, "rsp1");

	return true;
}

bool handle_fun2(std::string url, std::string body, mg_connection* c, OnRspCallback rsp_callback)
{
	// do sth
	std::cout << "handle fun2" << std::endl;
	std::cout << "url: " << url << std::endl;
	std::cout << "body: " << body << std::endl;
	rsp_callback(c, "rsp2");
	return true;
}

int _tmain(int argc, _TCHAR* argv[]) {

	std::string port = "8090";
	HttpServer http_server;
	http_server.Init(port);
	// add handler
	http_server.AddHandler("/fun1", handle_fun1);
	http_server.AddHandler("/fun2", handle_fun2);
	http_server.Start();

	char s[100];
	gets_s(s);
    return 0;
}