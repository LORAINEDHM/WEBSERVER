#include "utils.hpp"

static int stoi( std::string & s ) {
    int i;
    std::istringstream(s) >> i;
    return i;
}


void Data::setServer(std::vector<ServerPars> & servers, int listenFd) {
	
	std::vector<ServerPars>::iterator it;
	for(it = servers.begin(); it != servers.end(); it++) {
			if ((*it).getFd() == listenFd) {
			this->server = *it;
		}
	}
}

int get_listener_socket(char* port) {
	

	//  *******************************************************************
	//  **        CREATE LISTENING SOCKETS : SOCKET, BIND, LISTEN        **
	//  *******************************************************************
	
	int status, listenFd;
	int yes = 1;
	struct addrinfo hints, *res, *p;

		memset(&hints, 0, sizeof hints);
		memset(&res, 0, sizeof res);
		memset(&p, 0, sizeof p);
		hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE; // fill in host IP
		if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
			std::cerr << "port error" << std::endl;
			exit(1);
		}

		for(p = res; p != NULL; p = p->ai_next) {
			listenFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (listenFd < 0) {
				std::cerr << "socket error" << std::endl;
				continue;
			}
			// set socket as non blocking
			fcntl(listenFd, F_SETFL, O_NONBLOCK);
		
			// Lose the pesky "address already in use" error message
			setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
			
			if (bind(listenFd, p->ai_addr, p->ai_addrlen) < 0) {
				std::cerr << "bind error" << std::endl;
				close(listenFd);
 				continue;
 			}

 			break;
		}
		freeaddrinfo(res);

		if (p == NULL)  // means we didn't get bound
			return -1;
		
		if (listen(listenFd, 10) == -1) {
			std::cerr << "listen error" << std::endl;
			return -1;
		}
	return listenFd;
}

// Add a new file descriptor to the set
void add_to_pfds(std::vector<struct pollfd> &pfds, int newfd)
{
	struct pollfd newPfd;
	newPfd.fd = newfd;
	newPfd.events = POLLIN; // Check ready-to-read
	pfds.push_back(newPfd);
}

int is_listener(std::vector<ServerPars> servers, int fd) {
	std::vector<ServerPars>::iterator serv;
	for(serv = servers.begin(); serv != servers.end(); serv++) {
		if ((*serv).getFd() == fd)
			return (*serv).getFd();
	}
	return 0;
}

// Get sockaddr
void *get_in_addr(struct sockaddr *sa) {
	
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int store_header_request(Data &recvData) {

    std::string headerBreak = "\r\n\r\n";
    size_t found;

    recvData.header.append(recvData.buf);
    if ((found = recvData.header.find(headerBreak)) != std::string::npos) {
        recvData.headerBreak = true;


        // if char after headerbreak, erase from header and add it to body
        if (recvData.header[found + 4] != *(recvData.header.end())) {
            try
            {
                recvData.body.append(recvData.header.substr(found + 4));
                recvData.header.resize(found + 4);
            }
            catch (std::exception & e)
            {
                std::cout << "Exception catched : " << e.what() << std::endl << std::endl;
            }

        }

		// parse the request to get length;
		HttpRequest request(recvData.header);
		if (request.method == "POST") {
			// get body size
			recvData.bodyLen = stoi(request._headers["Content-Length"]);
			if (recvData.bodyLen > recvData.server.getBody()) {
				recvData.body.clear();
				return 1;
			}
			recvData.totalBytes = (int)recvData.body.length();
			recvData.restBytes = recvData.bodyLen - recvData.totalBytes;
			if (recvData.restBytes <= 0) {
				return 1;
			}
		}
		else  {
			return 1;
		}
	}
	return 0;
}

int store_body_request(Data &recvData) {

	recvData.body.append(recvData.buf, recvData.nbytes);
	recvData.totalBytes += recvData.nbytes;
	recvData.restBytes = recvData.bodyLen - recvData.totalBytes;
	if (recvData.restBytes <= 0) {
		return 1;
	}
	return 0;
}

Data& update_allData(std::map<int, Data> &allData, int fd) {

	if (allData.find(fd) == allData.end()) {

		// create new data object
		Data newData;
		allData.insert(std::pair<int, Data>(fd, newData));
	}
	return(allData[fd]);
}

int handle_recvData(Data &recvData, std::vector<struct pollfd> &pfds, int i, int &fdCount) {

	int requestCompleted;
	int senderFd = pfds[i].fd;
	memset(recvData.buf, 0, sizeof recvData.buf);   // clear reading buf
	recvData.headerBreak = false;
	recvData.nbytes = recv(senderFd, recvData.buf, sizeof recvData.buf, 0);

	if (recvData.nbytes <= 0) {
		// Got error or connection closed by client
		
		if (recvData.nbytes == 0) {
			// Connection closed
			std::cout << "pollserver: socket %d hung up\n" << senderFd << std::endl;
		} else {
			std::cerr << "recv error" << std::endl;
		}
		close(senderFd); // Bye!
		pfds.erase(pfds.begin() + i); // delete fd from pfds
		--fdCount;
	
	} else {
		if (recvData.headerBreak == false) {
			if ((requestCompleted = store_header_request(recvData) == 1)) {
				return 1;
			}
		}
		else
			if ((requestCompleted = store_body_request(recvData) == 1))
				return 1;
	}
	return 0;
}
