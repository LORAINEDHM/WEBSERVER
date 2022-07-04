#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include "utils.hpp"

class   ServerConf
{
    public:

    ServerConf();
    ServerConf(const char *path);
    ServerConf(ServerConf const & copy);

    ServerConf	&operator=( const ServerConf &other );

    int             checkConf(std::stringstream & buffer);
    std::string     cleanConf(std::string str);
    int             checkPars(std::string str);

    void    setNb();
    void    setNbserver();

    std::vector<ServerPars>     getServers();
    int                         getNb();
    int                         getNbServer();

    void        getVecserv();
    ServerPars  getServ(int nb);

    ~ServerConf();

    std::vector<ServerPars>     _servers;
    int                         _nb;
    int                         _nbserver;
    std::vector<std::string>    _vecserv;
    ServerPars                  _serv[100]; // new version;
};

std::ostream &operator<<(std::ostream &o, ServerConf const &rhs);

#endif