#include <utility>
#include "http_server.h"

std::string HttpServer::s_web_dir = ""; // ��ҳ��Ŀ¼ 
mg_serve_http_opts HttpServer::s_server_option; // web������ѡ��
std::unordered_map<std::string, ReqHandler> HttpServer::s_handler_map; // �ص�����ӳ���

void HttpServer::Init(const std::string &port)
{
	m_port = port;
	s_server_option.enable_directory_listing = "yes";
	s_server_option.document_root = s_web_dir.c_str();
 
	// ����http����
 
	// ���� CORS������ֻ�����ҳ������Ч
	// s_server_option.extra_headers = "Access-Control-Allow-Origin: *";
}
 
bool HttpServer::Start()
{
	mg_mgr_init(&m_mgr, NULL);
	mg_connection *connection = mg_bind(&m_mgr, m_port.c_str(), HttpServer::OnHttpWebsocketEvent);
	if (connection == NULL)
		return false;
	// for both http and websocket
	mg_set_protocol_http_websocket(connection);

	printf("starting http server at port: %s\n", m_port.c_str());
	// loop
	while (true)
		mg_mgr_poll(&m_mgr, 500); // ms
 
	return true;
}
 
void HttpServer::OnHttpWebsocketEvent(mg_connection *connection, int event_type, void *event_data)
{
	// ����http��websocket
	if (event_type == MG_EV_HTTP_REQUEST)
	{
		http_message *http_req = (http_message *)event_data;
		HandleHttpEvent(connection, http_req);
	}
	else if (event_type == MG_EV_WEBSOCKET_HANDSHAKE_DONE ||
		     event_type == MG_EV_WEBSOCKET_FRAME ||
		     event_type == MG_EV_CLOSE)
	{
		websocket_message *ws_message = (struct websocket_message *)event_data;
	}
}
 
// ---- simple http ---- //
static bool route_check(http_message *http_msg, char *route_prefix)
{
	if (mg_vcmp(&http_msg->uri, route_prefix) == 0)
		return true;
	else
		return false;
 
	// TODO: �������ж� GET, POST, PUT, DELTE�ȷ���
	//mg_vcmp(&http_msg->method, "GET");
	//mg_vcmp(&http_msg->method, "POST");
	//mg_vcmp(&http_msg->method, "PUT");
	//mg_vcmp(&http_msg->method, "DELETE");
}
 
void HttpServer::AddHandler(const std::string &url, ReqHandler req_handler)
{
	if (HttpServer::s_handler_map.find(url) != HttpServer::s_handler_map.end())
		return;
 
	HttpServer::s_handler_map.insert(std::make_pair(url, req_handler));
}
 
void HttpServer::RemoveHandler(const std::string &url)
{
	auto it = HttpServer::s_handler_map.find(url);
	if (it != HttpServer::s_handler_map.end())
		HttpServer::s_handler_map.erase(it);
}
 
void HttpServer::SendHttpRsp(mg_connection *connection, std::string rsp)
{
	// --- δ����CORS
	// �����ȷ���header, ��ʱ��������HTTP/2.0
	mg_printf(connection, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
	// ��json��ʽ����
	mg_printf_http_chunk(connection, "{ \"result\": %s }", rsp.c_str());
	// ���Ϳհ��ַ��죬������ǰ��Ӧ
	mg_send_http_chunk(connection, "", 0);
}
 
void HttpServer::HandleHttpEvent(mg_connection *connection, http_message *http_req)
{
	std::string req_str = std::string(http_req->message.p, http_req->message.len);
	// �ȹ����Ƿ���ע��ĺ����ص�
	std::string url = std::string(http_req->uri.p, http_req->uri.len);
	std::string body = std::string(http_req->body.p, http_req->body.len);
	auto it = s_handler_map.find(url);
	if (it != s_handler_map.end())
	{
		ReqHandler handle_func = it->second;
		handle_func(url, body, connection, &HttpServer::SendHttpRsp);
	}
 
	// ��������
	{
		HttpServer::SendHttpRsp(connection, "welcome to httpserver");
	}
}
 