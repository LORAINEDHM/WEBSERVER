#include "utils.hpp"

# define DEFAULT_CONF "default.conf"

int main(int ac, char **av)
{
    std::string path;
	std::vector<ServerPars> servers;

	if (ac != 2)
	{
		std::cerr << "Usage : ./webserver {resource}" << std::endl;
		return 1;
	}
	path = (DEFAULT_CONF);
    ServerConf    server_conf((char *)path.c_str());
	servers = server_conf.getServers();

	ServerPars serv_to_use;
	for (std::vector<ServerPars>::iterator it = servers.begin();
			it != servers.end(); it++)
	{
		std::cout << "Port :" << it->getListen() << std::endl;
		if (it->getListen() == "82")
		{
			std::cout << "port 82 FOUND !" << std::endl;
			serv_to_use = *it;
			break ;
		}
	}
	std::ostringstream istring;
	istring << "GET " << av[1] << " HTTP/1.1\r\n"
		"Host: 127.0.0.1:82\r\n"
		"User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:97.0) Gecko/20100101 Firefox/97.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
		"Accept-Language: en-US,en;q=0.5\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Connection: keep-alive\r\n"
		"Upgrade-Insecure-Requests: 1\r\n"
		"Sec-Fetch-Dest: document\r\n"
		"Sec-Fetch-Mode: navigate\r\n"
		"Sec-Fetch-Site: none\r\n"
		"Sec-Fetch-User: ?1\r\n"
		"Pragma: no-cache\r\n"
		"Cache-Control: no-cache\r\n\r\n";
	std::string req_string(istring.str());

	std::cout << std::endl << "Serv to Use Root = " << serv_to_use.getRoot() << std::endl;

	std::cout << "req_string = [" << req_string << "]" << std::endl;

	HttpRequest request(req_string);

	std::cout << "Building response" << std::endl;

	HttpResponse response(request, serv_to_use);

	std::cout << "RESPONSE :" << std::endl << response.format() << std::endl;
	return 0;
}
