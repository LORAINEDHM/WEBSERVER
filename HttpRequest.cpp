#include "HttpRequest.hpp"

// NOW : Simple request class with simple error handling.
// TODO : Improve error handling.

HttpRequest::HttpRequest( std::string & str )
{
	statusCode = 0;
	// If there is no CRLF CRLF in the file, set the status to '400, Bad Request'.
	if (str.find("\r\n\r\n") == std::string::npos)
	{
		statusCode = 400;
		return ;
	}
	std::istringstream data(str);
	std::string line;
	if (getline(data, line))
	{
		// Trimming the line.
		line.erase(0, line.find_first_not_of(" \r\n\t"));
		line.erase(line.find_last_not_of(" \r\n\t") + 1);


		//	 METHOD [...] URI [...] VERSION
		//	 |     |
		// pos     end
		size_t pos = 0;
		size_t end_pos = line.find(" ");

		if (end_pos == std::string::npos)
		{
			statusCode = 400;
			return ;
		}

		method = line.substr(pos, end_pos - pos);

		//	 METHOD [...] URI [...] VERSION
		//				  |  |
		//  			pos  end
		pos = line.find_first_not_of(" ", end_pos);
		end_pos = line.rfind(" ");

		if (end_pos <= pos)
		{
			statusCode = 400;
			return ;
		}

		uri = line.substr(pos, end_pos - pos);
		getURLvar(uri);

		//	 METHOD [...] URI [...] VERSION
		//							|      |
		//  					  pos      end
		pos = line.find_first_not_of(" ", end_pos);
		end_pos = line.length();

		version = line.substr(pos, end_pos - pos);
	}
	while (getline(data, line))
	{
		// If line is empty, we have found the CRLF between the request's headers and body.
		if (line == "\r")
			break ;

		// Trimming the line.
		line.erase(0, line.find_first_not_of(" \r\n\t"));
		line.erase(line.find_last_not_of(" \r\n\t") + 1);

		// DEBUG
		//std::cout << line << std::endl;

		//	Headers in HTTP request are sent like this :
		//		{Header}: [...] {value}
		//	For each line, find the separator (':') between the header and its value
		// 		and add them to the headers' map.
		size_t pos = line.find(":");
		if (pos != std::string::npos)
		{
			_headers[line.substr(0, pos)] = line.substr(line.find_first_not_of(" \t",
												line.find_first_not_of(":", pos)));
		}
		else
			std::cout << std::endl;
	}
	_body = std::string(data.str(), data.str().find("\r\n\r\n") + 4);

// For debug :
	//	std::cout << "Method = [" << method << "]" << std::endl;
	//	std::cout << "Path = [" << uri << "]" << std::endl;
	//	std::cout << "Version = [" << version << "]" << std::endl;
	//	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	//		std::cout << it->first << " => [" << it->second << "]" << std::endl;
	//	std::cout << "Body :" << std::endl << _body << std::endl;
}


//	NOW :
//		Get the query string, store it in a string and split the URL variables in a map.
//	TODO :
//		Get the query string and store it in a string.
void HttpRequest::getURLvar(std::string &uri)
{
	size_t pos = uri.find("?");
	if (pos != std::string::npos)
		queryString.assign(uri, pos + 1, uri.length() - pos - 1);

	while (pos != std::string::npos)
	{
		size_t equal = uri.find("=");
		size_t end = uri.find("&", pos + 1);
		if (end == std::string::npos)
			end = uri.length();
		if (equal == std::string::npos)
			equal = end;
		if (++pos != end)
		{
			if (equal != end)
				var[std::string(uri, pos, equal - pos)] = std::string(uri, equal + 1, end - equal - 1);
			else
				var[std::string(uri, pos, equal - pos)] = std::string();
		}
		uri.erase(pos - 1, end + 1 - pos);
		pos = uri.find_first_of("?&");
	}
}

HttpRequest::~HttpRequest(void)
{
	return ;
}
