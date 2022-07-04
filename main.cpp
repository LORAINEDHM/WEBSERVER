#include "utils.hpp"
#include <csignal>
int get_listener_socket(char* port);
void add_to_pfds(std::vector<struct pollfd> &pfds, int newfd);
void add_listener_pfds(std::vector<ServerPars> servers, std::vector<struct pollfd> &pfds);
int is_listener(std::vector<ServerPars> servers, int fd);
void *get_in_addr(struct sockaddr *sa);
int store_header_request(Data &recvData);
int store_body_request(Data &recvData);
Data& update_allData(std::map<int, Data> &allData, int fd);
int handle_recvData(Data &recvData, std::vector<struct pollfd> &pfds, int i, int &fdCount);

# define DEFAULT_CONF "default.conf"

void signalHandler(int signum)
{
	exit(signum);
}

int main(int argc, char **argv)
{
	signal(SIGINT, signalHandler);

	std::string path;
	std::vector<ServerPars> servers;

	if (argc > 2)
		return (-1);
	else if (argc == 1) 
		path = (DEFAULT_CONF);
	else
		path = (argv[1]);
	FILE* file = fopen(path.c_str(), "r");
	if (file == NULL)
	{
		std::cerr << "Cannot open file: " << path << std::endl;
		return (-1);
	}
	fclose(file);
	ServerConf    server_conf((char *)path.c_str());
	servers = server_conf.getServers();



	/*
	 ** ---------- create sockets for each port --------------
	 ** ---------- add listen_fd into pfds struct ------------
	 */

	int listenFd;
	std::vector<struct pollfd> pfds;

	std::vector<ServerPars>::iterator port;
	for(port = servers.begin(); port != servers.end(); port++) {
		if ((listenFd = get_listener_socket(const_cast<char*>((*port).getListen().c_str()))) < 0)
			exit(1);
		(*port).setFd(listenFd);
		std::cout << "    listening on port : " << (*port).getListen() << std::endl; 
		add_to_pfds(pfds, listenFd);
	}

	std::cout << " ******************************* waiting for connection ***********************************" << std::endl;

	/*
	 ** ------------------------ poll ------------------------
	 */

	struct sockaddr_storage remoteaddr; // Client address
	socklen_t addrLen;
	int connFd, pollCount, fdCount;
	std::map<int, Data> allData;

	while (1) {

		int listener;

		fdCount = pfds.size();
		if ((pollCount = poll(pfds.data(), fdCount, -1)) == -1) {
			std::cerr << "poll error" << std::endl;
			exit(1);
		}
		// Run through the existing connections looking for data to read
		for(int i = 0; i < fdCount; i++) {

			// Check if someone's ready to write
			if (pfds[i].revents & POLLOUT) {
				// If not the listener & POLLOUT, we're just a regular client
				// send data (write response into connFd)

				if (allData[pfds[i].fd].to_send == 0)
				{
					std::string totalRequest = allData[pfds[i].fd].header + allData[pfds[i].fd].body;

					HttpRequest requestOff(totalRequest);
					HttpResponse response(requestOff, allData[pfds[i].fd].server);
					allData[pfds[i].fd].str_to_send = response.format();
					allData[pfds[i].fd].to_send = allData[pfds[i].fd].str_to_send.length();
				}
				ssize_t sent;
				if ((sent = send(pfds[i].fd, allData[pfds[i].fd].str_to_send.c_str() , allData[pfds[i].fd].str_to_send.length(), 0)) <= 0)
				{
					std::cerr << "Send error" << std::endl;
				}
				allData[pfds[i].fd].to_send -= sent;
				if (sent > 0 && allData[pfds[i].fd].to_send)
				{
					allData[pfds[i].fd].str_to_send.erase(0, sent);
					pfds[i].events = POLLOUT;
				}
				else
				{
					// Remove client
					allData.erase(pfds[i].fd);
					close(pfds[i].fd);
					pfds.erase(pfds.begin() + i);

					--fdCount;
				}

			} else if (pfds[i].revents & POLLIN) {
				// Check if someone's ready to rea

				if ((listener = is_listener(servers, pfds[i].fd)) != 0) {
					//If listener is ready to read, handle new connection

					addrLen = sizeof remoteaddr;
					connFd = accept(listener, (struct sockaddr *)&remoteaddr, &addrLen);

					if (connFd == -1)
						std::cerr << "accept error" << std::endl;
					else {
						update_allData(allData, connFd).setServer(servers, listener);
						add_to_pfds(pfds, connFd);
						fdCount++;
						// std::cout << "  new connection " << std::endl;
						// printf("pollserver: new connection from %s on ""socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), connFd);
					}
				} else {
					// If not the listener & POLLIN, we're just a regular client

					// check if data exists for current fd; if not, create Data object pair and add it to all_data map;
					// update all_data returns data oject
					if (handle_recvData(update_allData(allData, pfds[i].fd), pfds, i, fdCount)) {
						// means request completed => go to response !
						pfds[i].events = POLLOUT;
					}
				}

			} 
		}
	}

	return(0);
}
