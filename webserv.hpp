#include "utils.hpp"

class Data {

	public:
	ServerPars server;
	std::string header;
	std::string body;
	std::string str_to_send;
	ssize_t to_send;
	int nbytes;
	int restBytes;
	int totalBytes;
	int bodyLen;
	char buf[4096];
	bool headerBreak;
	Data() : str_to_send(""), to_send(0), totalBytes(0), headerBreak(false) {};
	~Data() {};
	void setServer(std::vector<ServerPars> & servers, int listenFd);

};
