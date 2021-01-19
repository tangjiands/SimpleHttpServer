# SimpleHttpServer
基于mongoose 
借用了以下代码
https://blog.csdn.net/u012234115/article/details/79596826

稍微做了修改 支持标准c++  使用VS2010编译

使用也很简单:


bool handle_fun1(std::string url, std::string body, mg_connection* c, OnRspCallback rsp_callback)
{
	// do sth
	std::cout << "handle fun1" << std::endl;
	std::cout << "url: " << url << std::endl;
	std::cout << "body: " << body << std::endl;

	rsp_callback(c, "rsp1");

	return true;
}

int _tmain(int argc, _TCHAR* argv[]) 
{

	std::string port = "8090";
	HttpServer http_server;
	http_server.Init(port);
	// add handler
	http_server.AddHandler("/fun1", handle_fun1);
	http_server.Start();
        return 0;
}


注意:只支持GET!!!!
