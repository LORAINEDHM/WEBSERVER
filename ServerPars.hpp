#ifndef SERVERPARS_HPP
#define SERVERPARS_HPP

#include "utils.hpp"

class ServerPars;

class   Location
{
    public:

    Location();
    Location(ServerPars server);
    Location(int nb, int num, std::string str);
    Location(Location const & copy);

    ~Location();

    Location	&operator=( const Location &other );

    int     checkLocation(std::string str);
    int     checkMethod(std::string str);
    int     checkRedirect(std::string str);
    int     checkUpload(std::string str);
    int     checkLIndex(std::string str);
    int     checkLAutoIndex(std::string str);
    int     checkFastcgi(std::string str);
    int     checkFastpass(std::string str);

    void    setLnb(int nb);
    void    setLocation(std::string str);

    int                                 const & getLnb() const;
    int                                 const & geLnum() const;
    std::string                         const & getLstr() const;
    std::string                         const & getLocation() const;
    std::vector<std::string>            const & getMethod() const;
    std::string                         const & getRedirect() const;
    std::string                         const & getUpload() const;
    std::string                         const & getLautoindex() const;
    std::list<std::string>              const & getIndex() const;
    std::map<std::string, std::string>  const & getFastcgi() const;
    std::string                         const & getFastpass() const;

    class WrongLocation : public std::exception
	{
		public:
			virtual const char *what() const throw()
			{
				return ("location/.conf not good!");
			}
	};

    protected:

    int                                 _lnb; 
    int                                 _lnum; 
    std::string                         _lstr; 
    std::string                         _location;
    std::vector<std::string>            _method;
    std::string                         _redirection;
    std::string                         _upload;
    std::string                         _lautoindex;
    std::list<std::string>              _lindex;
    std::map<std::string, std::string>  _fastcgi_param;
    std::string                         _fastcgi_pass;
};

std::ostream &operator<<(std::ostream &o, Location const &rhs);

class   ServerPars: public Location
{
    public:

    ServerPars();
    ServerPars(int nb, std::string str);
    ServerPars(ServerPars const & copy);

    ~ServerPars();

    ServerPars	&operator=( const ServerPars &other );

    int     checkListen(std::string str);
    int     checkName(std::string str);
    int     checkError(std::string str);
    int     checkBody(std::string str);
    int     checkRoot(std::string str);
    int     checkIndex(std::string str);
    int     checkAutoIndex(std::string str);
    int     checkSmethod(std::string str);
    int     checkSupload(std::string str);

    void    setNb(int & nb);
    void    setStr(std::string & str);
    void    setFd(int & listenFd);

    std::vector<Location>       const & getLocations() const;  
    int                         const & getFd() const;
    int                         const & getNb() const;
    std::string                 const & getStr() const;
    std::string                 const & getListen() const;
    std::string                 const & getAddress() const;
    std::list<std::string>      const & getName() const;
    std::map<int, std::string>  const & getError() const;
    int                         const & getBody() const;
    std::string                 const & getRoot() const;
    std::string                 const & getAutoindex() const;
    std::list<std::string>      const & getIndex() const;
    std::vector<std::string>    const & getSmethod() const;
    std::string                 const & getSupload() const;

    class WrongServer : public std::exception
	{
		public:
			virtual const char *what() const throw()
			{
				return ("server/.conf not good!");
			}
	};

    private:

    std::vector<Location>               _locations;
    int                                 _fd;
    int                                 _nb;
    int                                 _nbloc;
    std::string                         _str;
    std::string                         _listen;
    std::string                         _address;
    std::list<std::string>              _server_name;
    std::string                         _default_name;
    std::map<int, std::string>          _error_page;
    int                                 _client_max_body_size;
    std::string                         _root;
    std::string                         _autoindex;
    std::list<std::string>              _index;
    std::vector<std::string>            _smethod;
    std::string                         _supload;
};

std::ostream &operator<<(std::ostream &o, ServerPars const &rhs);

#endif
