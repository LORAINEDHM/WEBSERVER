#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <cstring>
# include <string>
# include <sstream>
# include <fstream>
# include <errno.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdint.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/wait.h>

# include "utils.hpp"

class Cgi
{
	private:
		// CGI Environment Variables
		std::string	_CONTENT_LENGTH;	// Request body length.
		std::string _GATEWAY_INTERFACE;	// The revision of the Common Gateway Interface that the server uses.
		std::string _PATH_INFO;			// Extra path information passed to a CGI program.
		std::string _PATH_TRANSLATED;	// root of config + PATH_INFO
		std::string _QUERY_STRING;		// Query information.
		std::string _REMOTE_ADDR;		// Remote IP address of the client making the request;
		std::string _REMOTE_USER;		// User's login, authenticated by the web server.
		std::string _REQUEST_METHOD;	// HTTP request method used for this request.
		std::string _SCRIPT_FILENAME;	// URL path of the script being executed.
		std::string _SCRIPT_NAME;		// URL path of the script being executed.
		std::string _SERVER_NAME;		// Server's hostname or IP address.
		std::string _SERVER_PROTOCOL;	// Name and version of the request protocol, e.g., "HTTP/1.1"
		std::string _SERVER_PORT;		// Port number of the host.
		std::string _SERVER_SOFTWARE;	// Name and version of the answering server.

		std::string _root;
		std::string _body;
		std::string _env[14];

		Cgi();
		Cgi(Cgi const &);
		Cgi& operator=(Cgi const &);

	public:
		Cgi( HttpRequest const & request, ServerPars & server );
		~Cgi();

		int run(HttpRequest const & request);
		std::string const & getBody(void);
};

#endif
