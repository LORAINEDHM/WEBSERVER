#include "utils.hpp"

ServerConf::ServerConf(){}

ServerConf::ServerConf(const char *path)
{
    _nb = 0;
    _nbserver = 0;
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    if (checkConf(buffer) == -1)
        std::perror("Bad .conf!");
} 

ServerConf::ServerConf(ServerConf const & copy){(void)copy;}

ServerConf::~ServerConf(){}

int    ServerConf::checkConf(std::stringstream & buffer)
{
    std::string str;
    str = buffer.str();
    str = cleanConf(str);
    if (!str.compare(""))
        return (-1);
    if (checkPars(str) == (-1))
        return (-1);
    return (0);
}

int  ServerConf::checkPars(std::string str)
{
    int         a;
    int         i;
    int         size;
    int         bracket;
    std::string parsd;

    a = 0;
    while (a < _nb)
    {
        bracket = 0;
        i = 0;
        while (str[i])
        {
            if (str[i] == '{')
                bracket++;
            else if (str[i] == '}')
                bracket--;
            if (bracket < 0)
                break ;
            if (bracket == 0)
                break ;
            i++;
        }
        parsd = str.substr(0, (i + 1));
        ServerPars  _serv(a, parsd);
        _servers.push_back(_serv);
        _vecserv.push_back(parsd);
        size = parsd.size();
        str.erase(0, size);
        a++;
    }
    for (std::vector<ServerPars>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        std::string comp = (*it).getListen();
        for (std::vector<ServerPars>::iterator jt = it + 1; jt != _servers.end(); jt++)
        {
            if (comp == (*jt).getListen())
            {
                std::cerr << "ERROR port already used !" << std::endl;
                exit(1);
            }
        }
    }
//    getVecserv();
    return (0);
}

std::string    ServerConf::cleanConf(std::string str)
{
    int                 i;
    int                 bracket;
    int                 a;
    int                 count;
    std::string         line;
    std::stringstream   strstream(str);

    bracket = 0;
    str = "";
    while(std::getline(strstream, line, '\n'))
    {
        line = line.substr(0, line.find("#"));
        str += line;
    }
    i = 0; a = 0; count = 0;
    while (str[i])
    {
        if (bracket == 0)
            count++;
        if (str[i] == '{')
            bracket++;
        else if (str[i] == '}')
        {
            if (bracket == 1)
                a = i;
            bracket--;
        }
        if (bracket < 0)
            break ;
        if ((bracket == 1) && (str[i] == '{'))
        {
            if (a != 0)
                a++;
            count--;
            str.erase(a, count);
            count = 0; a = 0;
            _nb++;
            i -= 5;
            continue ;
        }
        if ((bracket == 0) && (str[i + 1] == '\0'))
            str.erase(a + 1, count);
        i++;
    }
    if (bracket != 0)
        return ("");
    setNbserver();
    return (str);
}

void   ServerConf::setNbserver()
{
    _nbserver = _nb;
}

int    ServerConf::getNb()
{
    return(_nb);
}

std::vector<ServerPars>         ServerConf::getServers()
{
    return (_servers);
}

int    ServerConf::getNbServer()
{
    return(_nbserver);
}

void   ServerConf::getVecserv()
{
    for (std::vector<std::string>::iterator it = _vecserv.begin(); it != _vecserv.end(); it++)
        std::cout << "str = " << *it << std::endl;
}

ServerPars  ServerConf::getServ(int nb)
{
    return (_serv[nb]);
}