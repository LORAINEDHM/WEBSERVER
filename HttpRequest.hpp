#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <cstring>
#include <ctime>

//	TODO :
//		Set the public variables to private and create proper getters and setters.

class HttpRequest
{
	private:
		HttpRequest( void );
		HttpRequest( HttpRequest const & src );
		HttpRequest& operator=( HttpRequest const & rhs );

	public:
		// Status line
		std::string method;
		std::string uri;
		std::string version;

		// Http Headers
		std::map<std::string, std::string> _headers;

		// Body
		std::string _body;

		// URL variables
		std::string queryString;
		std::map<std::string, std::string> var;

		// Status Code
		unsigned int statusCode;

		HttpRequest( std::string & str );
		~HttpRequest( void );

		void getURLvar(std::string &);
};

#endif
