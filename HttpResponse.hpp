#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
// #include <experimental/filesystem>
# include <ctime>
# include <errno.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdint.h>
# include <stdio.h>
# include <time.h>
# include <dirent.h>
# include <sys/types.h>
# include <sys/stat.h>

# include "utils.hpp"
# include "ServerPars.hpp"
# include "HttpRequest.hpp"

# define IS_DIR 2
# define IS_FILE 4

class ServerPars;
class Location;

class HttpResponse
{
private:
	HttpResponse( void );
	HttpResponse( HttpResponse const & src );
	HttpResponse& operator=( HttpResponse const & rhs );

	// Status line
	std::string _version;
	int _statusCode;
	std::string _status;
	std::string _uri;

	// Response Headers
	std::map<std::string, std::string> _headers;
	std::string _host;

	// Body
	std::string _body;

	// Maps
	std::map<int, std::string> _statusMap;
	std::map<std::string, std::string> _contentTypeMap;
	std::map<std::string, std::string> _varMap;

	// URL variables
	std::map<std::string, std::string> _URLvar;

	// Server and Location parameters
	class ServerPars _server;
	class Location _location;
	std::string _root;

	struct stat _fileStat;

	void initContentTypeMap(void);
	void initStatusMap(void);

	class Location getLocation(void);

	bool isMethodAllowed(HttpRequest & request);
	bool checkContentType(std::string const & content_type);
	std::string getContentTypeExtension(std::string const & content_type);

	int checkRequest(HttpRequest & request);
	int methodGet(void);
	int methodPost(HttpRequest &);
	int methodDelete(void);

	unsigned int checkPermission(std::string, struct stat &, unsigned int);

public:
	HttpResponse( HttpRequest& , ServerPars & );
	~HttpResponse( void );

	std::string& getStatus(void);
	std::string getExtension(void);
	std::string& getContentType(void);
	std::string format(void);
};

#endif
