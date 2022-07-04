#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Cgi.hpp"


// 	DONE : GET, DELETE and POST methods + CGI. Server config added.
// 		Autoindex seems to work fine.
//		POST files to upload directory.
//		Redirections handling.
// 		Link statusCode to error pages.

HttpResponse::HttpResponse(void)
{
	return ;
}

class Location HttpResponse::getLocation(void)
{
	std::vector<Location> locations = _server.getLocations();
	std::string path(_uri);
	while (path.length())
	{
		size_t pos = path.find_last_of("/");
		for (std::vector<Location>::iterator it = locations.begin();
					it != locations.end(); it++)
		{
			std::string to_compare(it->getLocation());
			if (*(to_compare.begin()) != '/')
				to_compare.insert(0, "/");
			if (to_compare != "/" && *(to_compare.rbegin()) == '/')
				to_compare.erase(to_compare.end() - 1);
			if (path == to_compare)
				return *it;
		}
		if (path == "/")
			break ;
		if (pos == 0)
			++pos;
		path.erase(pos);
	}
	return (_server);
}


HttpResponse::HttpResponse( HttpRequest & request, ServerPars & server ) : _version("HTTP/1.1")
{
	initStatusMap();
	initContentTypeMap();

	_uri = request.uri;

	_statusCode = 0;

	_server = server;

//	std::cout << "RespServ Name :" << _server.getName().front() << std::endl;
//	std::cout << "RespServ FD :" << _server.getFd() << std::endl << std::endl;

//	std::cout << std::endl << "Getting location" << std::endl;
	_location = getLocation();

//	std::cout << "Location = [" << _location.getLocation() << "]" << std::endl;
//	std::cout << "\tAutoindex = [" << _location.getLautoindex() << "]" << std::endl;
//	std::cout << "\tRedirection : [" << _location.getRedirect() << "]" << std::endl;

	_root = _server.getRoot();

	if (*(_root.rbegin()) == '/')
		_root.erase(_root.length() - 1);


	_version = "HTTP/1.1";

//	std::cout << "_uri = " << _uri << std::endl;
//	std::cout << std::endl << "Before checking request : Status Code = " << _statusCode << std::endl;

	_statusCode = checkRequest(request);

	if (_statusCode != 200)
		return ;
	
//	std::cout << "After checking request : Status Code = " << _statusCode << std::endl << std::endl;

	if (*(_uri.begin()) != '/')
	{
		_statusCode = 400;
		return ;
	}

	_host = request._headers["Host"];

	if (_location.getRedirect().length())
	{
		_headers["Location"] = "http://" + _host + _location.getRedirect();
		_statusCode = 301;
	}

	if (_statusCode != 200)
		return ;

	_URLvar = request.var;

	if (*(_uri.rbegin()) == '/' && _location.getIndex().front().length())
		_uri += _location.getIndex().front();

	if (getExtension() == "cgi")
	{
		struct stat resource_stat;
		int perm = checkPermission(_root + _uri, resource_stat, S_IROTH | S_IXOTH);
		if (perm & IS_FILE && perm & true)
		{
			Cgi cgi(request, _server);
			_statusCode = cgi.run(request);
			if (_statusCode >= 200 && _statusCode < 300)
				_body = cgi.getBody();
			_headers["Content-Type"] = "text/html; charset=utf-8";

			getStatus();

			// Convert int to std::string.
			std::ostringstream convert;
			convert << _body.length();
			_headers["Content-Length"] = convert.str();
		}
		else if (_statusCode == 0)
		{
			_statusCode = 403;
		}
		return ;
	}
	if (request.method == "POST")
	{
		methodPost(request);
		return ;
	}
	if (request.method == "DELETE")
	{
		methodDelete();
		return ;
	}
	methodGet();

	// DEBUG
//	std::cout << "Status Code = " << _statusCode << std::endl;
//	std::cout << "Status = " << _status << std::endl;
}

HttpResponse::HttpResponse( HttpResponse const & src )
{
	*this = src;
	return ;
}

HttpResponse &HttpResponse::operator=( HttpResponse const & rhs )
{
	if (this != &rhs)
	{
		_version = rhs._version;
	}
	return *this;
}

unsigned int HttpResponse::checkPermission(std::string path_to_file, struct stat& resource, unsigned int mode_flags)
{
		// A function to check permission and type of a resource.
		// If first bit (right) is set, permission OK.
		// Sets 2nd bit if resource is a directory, 3rd bit if resource is a file.
		unsigned int ret = 0;

		// DEBUG
		//std::cout << "Path to file = [" << path_to_file << "]" << std::endl;

		if (stat(path_to_file.c_str(), &resource) == 0)
		{
			if (S_IFDIR & resource.st_mode)
			{
				ret = IS_DIR;
			}
			else if (S_IFREG & resource.st_mode)
			{
				ret = IS_FILE;
			}
			if (ret && (resource.st_mode & mode_flags) == mode_flags)
			{
				ret |= true;
				_statusCode = 200;
			}
			else
				_statusCode = 403;
		}
		else
		{
			// DEBUG
			//std::cout  << "STAT FAILED : " << strerror(errno);

			if (errno & EACCES)
			{
				// DEBUG
				//std::cout << " IF" << std::endl;

				_statusCode = 403;
			}
			else
			{
				// DEBUG
				//std::cout << " ELSE" << std::endl;

				_statusCode = 404;
			}
		}
		return (ret);
}

HttpResponse::~HttpResponse(void)
{
	return ;
}

std::string HttpResponse::format(void)
{
	// TODO : Clean the format function to work with all methods.
	time_t rawtime;
	struct tm* timeinfo;
	char date[128];
	char lastModif[128];

	time (&rawtime);
	timeinfo = gmtime(&rawtime);

	std::ostringstream oss;

	getStatus();

	// Set Date header string.
	strftime(date, 127, "Date: %a, %d %b %Y %H:%M:%S GMT", timeinfo);

	// Set Last-Modified header string.
	timeinfo = gmtime(&_fileStat.st_mtime);
	if (_statusCode >= 200 && _statusCode < 300 && _statusCode != 204 && getExtension() != "cgi")
		strftime(lastModif, 127, "Last-Modified: %a, %d %b %Y %H:%M:%S GMT", timeinfo);

	oss << _version << " " << _statusCode << " " << _status << "\r\n";
	oss << "Server: " << _server.getName().front() << "\r\n";

	//	If response has content (different than code 1XX, 204 and 304).
	// 		If statusCode is 'positive', add headers and body.
	// 		Else, statusCode not OK, generate status page.
	if (_statusCode >= 200 && _statusCode != 204 && _statusCode != 304)
	{
		if (_statusCode < 400)
		{
			oss << "Content-Length: " << _body.length() << "\r\n";
			if (_headers.count("Content-Type"))
				oss << "Content-Type: " << _headers["Content-Type"] << "\r\n";
			else
				oss << "Content-Type: " << getContentType() << "\r\n";
			oss << date << "\r\n";
			if (_statusCode == 201 || (_statusCode >= 300 && _statusCode < 400))
			{
				oss << "Location: " << _headers["Location"] << "\r\n";
			}
			else
			{
				oss << lastModif << "\r\n";
			}
			// CRLF sep between headers and body.
			oss << "\r\n";
			oss << _body;
		}
		else
		{
			// Generate a default html page.
			bool body_set = false;
			std::map<int, std::string>errorMap = _server.getError();
			std::ostringstream o_body;
			if (errorMap.count(_statusCode))
			{
				std::ifstream myfile;
				// DEBUG
				//std::cout << "Error page = [" << errorMap[_statusCode].c_str() << "]" << std::endl;
				myfile.open((errorMap[_statusCode]).c_str());
				if (myfile.is_open())
				{
					o_body << myfile.rdbuf();
					myfile.close();
					body_set = true;
				}
			}
			if (body_set == false)
			{
				o_body << "<head><title>" << _statusCode << " " << _status << "</title></head>";
				o_body << "<h1>" << _statusCode << " " << _status << "</h1>";

			}
			oss << "Content-Type: text/html; charset=utf-8\r\n";
			oss << "Content-Length: " << o_body.str().length() << "\r\n\r\n";
			oss << o_body.str();
		}
	}
	else
		oss << "\r\n";


	return oss.str();
}

bool HttpResponse::isMethodAllowed(HttpRequest & request)
{

	std::vector<std::string> allowed_methods_vec = _location.getMethod();

	for (std::vector<std::string>::iterator it = allowed_methods_vec.begin(); it != allowed_methods_vec.end(); it++)
	{
		std::string allowed_method(*it, 0, it->find_last_of(","));
		if (allowed_method == request.method)
			return true;
	}

	return false;
}

std::string HttpResponse::getContentTypeExtension(std::string const & content_type)
{
	for (std::map<std::string, std::string>::iterator it = _contentTypeMap.begin(); it != _contentTypeMap.end(); it++)
	{
		if (content_type.compare(it->second) == 0)
			return it->first;
	}
	std::cout << std::endl;
	return std::string("");
}

bool HttpResponse::checkContentType(std::string const & content_type)
{
	for (std::map<std::string, std::string>::iterator it = _contentTypeMap.begin(); it != _contentTypeMap.end(); it++)
	{
		if (content_type.compare(it->second) == 0)
			return (true);
	}
	std::cout << std::endl;
	return (false);
}

int HttpResponse::checkRequest(HttpRequest & request)
{
	if (request.statusCode)
		return request.statusCode;
	for (std::string::iterator it = request.method.begin(); it != request.method.end(); it++)
		if (!isupper(*it))
			return 400;
	if (request.method != "GET" && request.method != "DELETE" && request.method != "POST")
		return 501;

	if (isMethodAllowed(request) == false)
		return 405;

	if (request._headers.count("Content-Type"))
	{
		if (getExtension().compare("cgi") && checkContentType(request._headers["Content-Type"]) == false)
		{
			return 415;
		}
	}
	if (request._headers.count("Content-Length"))
	{
		int content_length;
		std::istringstream(request._headers["Content-Length"]) >> content_length;
		if (content_length > _server.getBody())
			return 413;
	}

	return 200;
}

std::string& HttpResponse::getStatus(void)
{
	_status = _statusMap[_statusCode];
	return _status;
}

std::string HttpResponse::getExtension(void)
{
	size_t last_slash = _uri.find_last_of("/");
	size_t last_dot = _uri.find_last_of(".");
	std::string extension("");
	if (last_dot != std::string::npos && (last_slash == std::string::npos || last_slash < last_dot))
		extension.assign(_uri, last_dot + 1, _uri.length() - last_dot - 1);
	return extension;
}

std::string& HttpResponse::getContentType(void)
{
	std::string extension;

	// If a redirection occurs, the response is an html page describing 
	// the status of the response.
	// Else, find the last dot after the last slash to get the extension.
	if (_statusCode >= 300)
		extension = "html";
	else
		extension = getExtension();

	// If an extension is found, the content type is set approprietly.
	// Else, the content type is set by default to 'bin'.
	if (_contentTypeMap.find(extension) != _contentTypeMap.end())
		return _contentTypeMap[extension];
	else
		return _contentTypeMap["bin"];
}

int HttpResponse::methodGet(void)
{
	std::ifstream myfile;
	std::ostringstream ss;

	if (_statusCode >= 200 && _statusCode < 300)
	{
		// Check if the resource is an accessible file.
		//	If the resource is a directory with the right perms and ending with '/'
		// 		try to find the default file.
		//	If the default file is not found nor set in the server config
		//		check if autoindex is set and print it.
		//	Else
		//		send a '403, Forbidden' status code.
		unsigned int perm = checkPermission(_root + _uri, _fileStat, S_IROTH);
		if (perm & IS_FILE)
		{
			myfile.open((_root + _uri).c_str());
			if (!myfile.is_open())
				_statusCode = 403;
		}
		else if (perm & true && perm & IS_DIR)
		{
			if (_location.getLautoindex() == "on")
			{
				std::string full_path(_root + _uri);
				DIR * dir;
				dir = opendir(full_path.c_str());

				if (*(_uri.rbegin()) != '/')
					_uri += '/';

				ss << "<!DOCTYPE html>\n<html>\n <body>\n  <h1>Index of " << _uri << "</h1>\n" << "</br></br>";

				struct dirent * directory_entry;
				while ((directory_entry = readdir(dir)))
				{
					if (full_path == _root && directory_entry->d_name == std::string(".."))
						continue ;
					ss << "<a href=\"http://localhost:" << _server.getListen() << _uri << directory_entry->d_name << "\">" << directory_entry->d_name << "</a></br>\n\n";
				}
				ss << " </body>\n</html>";

				_headers["Content-Type"] = "text/html; charset=utf-8";

				if (closedir(dir) < 0)
					_statusCode = 500;
			}
			// If directory without a ending '/'.
			else if (*(_uri.rbegin()) != '/')
			{
				_headers["Location"] = "http://" + _host + _uri + "/";
				_statusCode = 301;
			}
			else
				_statusCode = 403;
		}
	}
	getStatus();

	if (_statusCode >= 200 && _statusCode != 204 && _statusCode < 300)
	{
		ss << myfile.rdbuf();
		myfile.close();
	}
	_body = ss.str();
	return _statusCode;
}

int HttpResponse::methodPost(HttpRequest & request)
{
	if (_location.getUpload().empty() == false)
	{
		std::string upload_dir = _location.getUpload();
		std::string upload_path;
		if (*(upload_dir.begin()) == '/')
			upload_path = _root + _location.getUpload();
		else
			upload_path = _root +_location.getLocation() + "/" + _location.getUpload();
		if (*(upload_path.rbegin()) != '/')
			upload_path += '/';
		int perm = checkPermission(upload_path, _fileStat, S_IWOTH | S_IXOTH);
		if (perm & true)
		{
			if (perm & IS_DIR)
			{
					std::string filename;
					if (*(_uri.rbegin()) != '/')
						filename = _uri;
					else
					{
						time_t rawtime;
						struct tm* timeinfo;
						char c_filename[128];

						time (&rawtime);
						timeinfo = gmtime(&rawtime);

						strftime(c_filename, 127, "file_%a_%d_%b_%Y_%H:%M:%S", timeinfo);
						filename = c_filename;
						filename += "." + getContentTypeExtension(request._headers["Content-Type"]);
						_contentTypeMap["Content-Type"] = request._headers["Content-Type"];
						if (*(filename.rbegin()) == '.')
							filename.erase(filename.length() - 1);
					}
					size_t pos = filename.find_last_of('/');
					if (pos != std::string::npos)
					{
						filename = filename.erase(0, pos);
					}
						filename = upload_path + filename;

					std::ofstream new_file(filename.c_str());
					new_file << request._body;
					new_file.close();

					std::string location_str(filename, _root.length(), filename.length() - _root.length());

					this->_headers["Location"] = "http://" + _host + location_str;
					_statusCode = 201;
			}
			else
				_statusCode = 405;
		}
		else
			_statusCode = 403;
	}
	else
		_statusCode = 405;
	return _statusCode;
}

int HttpResponse::methodDelete(void)
{
	std::string file_path(_root + _uri);
	std::string dir_path(file_path, 0, (file_path).find_last_of("/"));
	struct stat dirStat;

	if (checkPermission(dir_path, dirStat, S_IWOTH | S_IXOTH) & true // Check directory permission
		&& checkPermission(file_path, _fileStat, S_IWOTH) & true)
	{
		if (remove(file_path.c_str()))
			_statusCode = 500;
		else
			_statusCode = 204;
	}
	getStatus();
	std::ostringstream ss;

	return _statusCode;
}

// TODO :	Move the status map and the file extensions map to the server config class
// 			and send a reference of those maps to the response class.

void HttpResponse::initStatusMap(void)
{
	// Informational
	_statusMap[100] = "Continue";
	_statusMap[101] = "Switching Protocols";

	// Successful;
	_statusMap[200] = "OK";
	_statusMap[201] = "Created";
	_statusMap[202] = "Accepted";
	_statusMap[203] = "Non-Authoritative Information";
	_statusMap[204] = "No Content";
	_statusMap[205] = "Reset Content";
	_statusMap[206] = "Partial Content";

	// Redirection;
	_statusMap[300] = "Multiple Choices";
	_statusMap[301] = "Moved Permanently";
	_statusMap[302] = "Found";
	_statusMap[303] = "See Other";
	_statusMap[304] = "Not Modified";
	_statusMap[305] = "Use Proxy";
	_statusMap[306] = "";
	_statusMap[307] = "Temporary Redirect";

	// Client Errors;
	_statusMap[400] = "Bad Request";
	_statusMap[401] = "Unauthorized";
	_statusMap[402] = "Payment Required";
	_statusMap[403] = "Forbidden";
	_statusMap[404] = "Not Found";
	_statusMap[405] = "Method Not Allowed";
	_statusMap[406] = "Not Acceptable";
	_statusMap[407] = "Proxy Authentication Required";
	_statusMap[408] = "Request Timeout";
	_statusMap[409] = "Conflict";
	_statusMap[410] = "Gone";
	_statusMap[411] = "Length Required";
	_statusMap[412] = "Precondition Failed";
	_statusMap[413] = "Request Entity Too Large";
	_statusMap[414] = "Request-URI Too Long";
	_statusMap[415] = "Unsupported Media Type";
	_statusMap[416] = "Requested Range Not Satisfiable";
	_statusMap[417] = "Expectation Failed";

	// Server Errors;
	_statusMap[500] = "Internal Server Error";
	_statusMap[501] = "Not Implemented";
	_statusMap[502] = "Bad Gateway";
	_statusMap[503] = "Service Unavailable";
	_statusMap[504] = "Gateway Timeout";
	_statusMap[505] = "HTTP Version Not Supported";


}

void HttpResponse::initContentTypeMap(void)
{
	_contentTypeMap["acc"] = "audio/aac";
	_contentTypeMap["abw"] = "application/x-abiword";
	_contentTypeMap["avi"] = "video/x-msvideo";
	_contentTypeMap["bin"] = "application/octet-stream";
	_contentTypeMap["bmp"] = "image/bmp";
	_contentTypeMap["bz"] = "application/x-bzip";
	_contentTypeMap["bz2"] = "application/x-bzip2";
	_contentTypeMap["csh"] = "application/x-csh";
	_contentTypeMap["css"] = "text/css";
	_contentTypeMap["csv"] = "text/csv";
	_contentTypeMap["doc"] = "application/msword";
	_contentTypeMap["gif"] = "image/gif";
	_contentTypeMap["htm"] = "text/html";
	_contentTypeMap["html"] = "text/html";
	_contentTypeMap["ico"] = "image/x-icon";
	_contentTypeMap["ics"] = "text/calendar";
	_contentTypeMap["jar"] = "application/java-archive";
	_contentTypeMap["jpeg"] = "image/jpeg";
	_contentTypeMap["jpg"] = "image/jpeg";
	_contentTypeMap["js"] = "application/javascript";
	_contentTypeMap["json"] = "application/json";
	_contentTypeMap["mid"] = "audio/midi";
	_contentTypeMap["midi"] = "audio/midi";
	_contentTypeMap["mpeg"] = "video/mpeg";
	_contentTypeMap["oga"] = "audio/ogg";
	_contentTypeMap["ogv"] = "video/ogg";
	_contentTypeMap["ogx"] = "application/ogg";
	_contentTypeMap["otf"] = "font/otf";
	_contentTypeMap["png"] = "image/png";
	_contentTypeMap["pdf"] = "application/pdf";
	_contentTypeMap["rar"] = "application/x-rar-compressed";
	_contentTypeMap["rtf"] = "application/rtf";
	_contentTypeMap["sh"] = "application/x-sh";
	_contentTypeMap["svg"] = "image/svg+xml";
	_contentTypeMap["tif"] = "image/tiff";
	_contentTypeMap["tiff"] = "image/tiff";
	_contentTypeMap["ts"] = "application/typescript";
	_contentTypeMap["ttf"] = "font/ttf";
	_contentTypeMap["txt"] = "text/plain";
	_contentTypeMap["vsd"] = "application/vnd.visio";
	_contentTypeMap["wav"] = "audio/x-wav";
	_contentTypeMap["weba"] = "audio/webm";
	_contentTypeMap["webm"] = "video/webm";
	_contentTypeMap["webp"] = "image/webm";
	_contentTypeMap["woff"] = "font/woff";
	_contentTypeMap["woff2"] = "font/woff2";
	_contentTypeMap["xhtml"] = "application/xhtml+xml";
	_contentTypeMap["xml"] = "application/xml";
	_contentTypeMap["zip"] = "application/zip";
}
