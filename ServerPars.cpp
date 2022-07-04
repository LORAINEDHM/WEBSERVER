#include "utils.hpp"

Location::Location()
{
}

Location::Location(ServerPars server)
{
    this->_location = "";
    this->_lautoindex = server.getAutoindex();
    this->_lindex = server.getIndex();
    this->_method = server.getSmethod();
    this->_upload = server.getSupload();
}

Location::Location(int nb, int num, std::string str): _lnb(nb), _lnum(num), _lstr(str)
{
    int         size;
    std::string parsed;

    if (str.find("location ") != str.npos)
    {
        size = 0;
        for (int i = str.find("location ");str[i] && str[i] != '{'; i++)
            size++;
        parsed = str.substr(str.find("location "), size);
        if (checkLocation(parsed) == (-1))
        {
            std::cerr << "ERROR .conf location::location !" << std::endl;
            exit(1);
        }
        str.erase(str.find("location "), size);
    }
    if (str.find("method ") != str.npos)
    {
        size = 0;
        for (int i = str.find("method ");str[i] && str[i] != ';'; i++)
        {
            size++;
            if (i + 1 == (int)str.size())
            {
                std::cerr << "ERROR .conf missing location method ';' !" << std::endl;
                exit(1);
            }
        }
        parsed = str.substr(str.find("method "), size);
        if (checkMethod(parsed) == (-1))
        {
            std::cerr << "ERROR .conf method !" << std::endl;
            exit(1);
        }
        str.erase(str.find("method "), size);
    }
    else if (str.find("method ") == str.npos)
    {
        std::cerr << "ERROR no method in the location !" << std::endl;
        exit(1);
    }
    if (str.find("redirection ") != str.npos)
    {
        size = 0;
        for (int i = str.find("redirection ");str[i] && str[i] != ';'; i++)
            size++;
        parsed = str.substr(str.find("redirection "), size);
        if (checkRedirect(parsed) == (-1))
        {
            std::cerr << "ERROR .conf redirection !" << std::endl;
            exit(1);
        }
        str.erase(str.find("redirection "), size);
    }
    if (str.find("upload ") != str.npos)
    {
        size = 0;
        for (int i = str.find("upload ");str[i] && str[i] != ';'; i++)
            size++;
        parsed = str.substr(str.find("upload "), size);
        if (checkUpload(parsed) == (-1))
        {
            std::cerr << "ERROR .conf upload !" << std::endl;
            exit(1);
        }
        str.erase(str.find("upload "), size);
    }
    if (str.find("autoindex ") != str.npos)
    {
        size = 0;
        for (int i = str.find("autoindex ");str[i] && str[i] != ';'; i++)
            size++;
        parsed = str.substr(str.find("autoindex "), size);
        if (checkLAutoIndex(parsed) == (-1))
        {
            std::cerr << "ERROR .conf location::autoindex !" << std::endl;
            exit(1);
        }
        str.erase(str.find("autoindex "), size);
    }
    if (str.find("index ") != str.npos)
    {
        size = 0;
        for (int i = str.find("index ");str[i] && str[i] != ';'; i++)
            size++;
        parsed = str.substr(str.find("index "), size);
        if (checkLIndex(parsed) == (-1))
        {
            std::cerr << "ERROR .conf location::index !" << std::endl;
            exit(1);
        }
        str.erase(str.find("index "), size);
    }
    while (str.find("fastcgi_param ") != str.npos)
    {
        size = 0;
        for (int i = str.find("fastcgi_param ");str[i] && str[i] != ';'; i++)
            size++;
        parsed = str.substr(str.find("fastcgi_param "), size);
        if (checkFastcgi(parsed) == (-1))
        {
            std::cerr << "ERROR .conf fastcgi_param !" << std::endl;
            exit(1);
        }
        str.erase(str.find("fastcgi_param "), size);
    }
    if (str.find("fastcgi_pass ") != str.npos)
    {
        size = 0;
        for (int i = str.find("fastcgi_pass ");str[i] && str[i] != ';'; i++)
            size++;
        parsed = str.substr(str.find("fastcgi_pass "), size);
        if (checkFastpass(parsed) == (-1))
        {
            std::cerr << "ERROR .conf fastcgi_pass !" << std::endl;
            exit(1);
        }
        str.erase(str.find("fastcgi_pass "), size);
    }
    size = 0;
    while (str[size])
    {
        if ((str[size] != ';') && (str[size] != '{') && (str[size] != '}') && (str[size] != ' ') && (str[size] != '\t') && (str[size] != '\n') && (str[size] != '\v') && (str[size] != '\f') && (str[size] != '\r'))
        {
            std::cerr << "ERROR .conf location !" << std::endl;
            exit(1);
        }
        size++;
    }
}

Location::Location(Location const & copy)
{
    this->_lnb = copy._lnb;
    this->_lnum = copy._lnum;
    this->_lstr = copy._lstr;
    this->_location = copy._location;
    this->_method = copy._method;
    this->_redirection = copy._redirection;
    this->_upload = copy._upload;
    this->_lautoindex = copy._lautoindex;
    this->_lindex = copy._lindex;
    this->_fastcgi_param = copy._fastcgi_param;
    this->_fastcgi_pass = copy._fastcgi_pass;
    *this = copy;
}

Location::~Location()
{
}

Location	& Location::operator=( const Location &other )
{
    if (this != &other)
    {
        this->_lnb = other.getLnb();
        this->_lnum = other.geLnum();
        this->_lstr = other.getLstr();
        this->_location = other.getLocation();
        this->_method = other.getMethod();
        this->_redirection = other.getRedirect();
        this->_upload = other.getUpload();
        this->_lautoindex = other.getLautoindex();
        this->_lindex = other.getIndex();
        this->_fastcgi_param = other.getFastcgi();
        this->_fastcgi_pass = other.getFastpass();
    }
    return *this;
}

int     Location::checkLocation(std::string str)
{
    int size;
    int j;
    
    while (str.find("location ") != str.npos)
    {
        size = 0;
        for (int i = str.find("location "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("location "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR location .conf !" << std::endl;
            exit(1);
        }
        _location += str[j];
        j++;
    }
    return (0);
}

int     Location::checkMethod(std::string str)
{
    int         size;
    std::string index;
    
    while (str.find("method ") != str.npos)
    {
        size = 0;
        for (int i = str.find("method "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("method "), size);
    }
    while (str != "")
    {
        size = 0;
        while (str[size] == ' ')
            size++;
        index = str.erase(str.find(" "), size);
        if (index == "")
            break ;
        size = 0;
        while (index[size])
        {
            if (index[size] == ' ')
                break ;
            size++;
        }
        index = index.substr(0, size);
        _method.push_back(index);
        str.erase(0, size);
    }
    for (std::vector<std::string>::iterator it = _method.begin(); it != _method.end(); it++)
    {
        if (((*it).compare("GET")) && ((*it).compare("POST")) && ((*it).compare("DELETE")))
        {
            std::cerr << "ERROR location method .conf !" << std::endl;
            exit(1);
        }
    }
    return (0);
}

int     Location::checkRedirect(std::string str)
{
    int size;
    int j;
    
    while (str.find("redirection ") != str.npos)
    {
        size = 0;
        for (int i = str.find("redirection "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("redirection "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR redirection .conf !" << std::endl;
            exit(1);
        }
        _redirection += str[j];
        j++;
    }
    return (0);
}

int     Location::checkUpload(std::string str)
{
    int size;
    int j;
    
    while (str.find("upload ") != str.npos)
    {
        size = 0;
        for (int i = str.find("upload "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("upload "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR location upload .conf !" << std::endl;
            exit(1);
        }
        _upload += str[j];
        j++;
    }
    return (0);
}

int     Location::checkLIndex(std::string str)
{
    int         size;
    std::string index;
    
    while (str.find("index ") != str.npos)
    {
        size = 0;
        for (int i = str.find("index "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("index "), size);
    }
    while (str != "")
    {
        size = 0;
        while (str[size] == ' ')
            size++;
        index = str.erase(str.find(" "), size);
        if (index == "")
            break ;
        size = 0;
        while (index[size])
        {
            if ((index[size] == ' ') || (index[size] == '\t') || (index[size] == '\n') || (index[size] == '\v') || (index[size] == '\f') || (index[size] == '\r'))
            {
                std::cerr << "ERROR location index .conf !" << std::endl;
                exit(1);
            }
            size++;
        }
        index = index.substr(0, size);
        _lindex.push_back(index);
        str.erase(0, size);
    }
    return (0);
}

int     Location::checkLAutoIndex(std::string str)
{
    int size;
    int j;
    
    while (str.find("autoindex ") != str.npos)
    {
        size = 0;
        for (int i = str.find("autoindex "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("autoindex "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR location autoindex .conf !" << std::endl;
            exit(1);
        }
        _lautoindex += str[j];
        j++;
    }
    if ((_lautoindex.compare("off")) && (_lautoindex.compare("on")))
    {
        std::cerr << "ERROR location autoindex .conf !" << std::endl;
        exit(1);
    } 
    return (0);
}

int     Location::checkFastcgi(std::string str)
{
    std::string         fastcgi;
    std::string         path;
    int                 size;

    while (str.find("fastcgi_param ") != str.npos)
    {
        size = 0;
        for (int i = str.find("fastcgi_param "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("fastcgi_param "), size + 1);
    }
    size = 0;
    while (str[size] && (str[size] != '$'))
    {
        if (str[size] != ' ')
            fastcgi += str[size];
        size++;
    }
    while (str[size])
    {
        if ((str[size] == ' ') || (str[size] == '\t') || (str[size] == '\n') || (str[size] == '\v') || (str[size] == '\f') || (str[size] == '\r'))
        {
            std::cerr << "ERROR fastcgi_param .conf !" << std::endl;
            exit(1);
        }
        path += str[size];
        size++;
    }
    _fastcgi_param.insert(std::pair<std::string, std::string>(fastcgi, path));
    return (0);
}

int     Location::checkFastpass(std::string str)
{
    int size;
    int j;
    
    while (str.find("fastcgi_pass ") != str.npos)
    {
        size = 0;
        for (int i = str.find("fastcgi_pass "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("fastcgi_pass "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR fastcgi_pass .conf !" << std::endl;
            exit(1);
        }
        _fastcgi_pass += str[j];
        j++;
    }
    return (0);
}

void    Location::setLnb(int nb)
{
    _lnb = nb;
}

void    Location::setLocation(std::string str)
{
    _location = str;
}


int                                 const & Location::getLnb() const
{
    return (_lnb);
}

int                                 const & Location::geLnum() const
{
    return (_lnum);
}

std::string                         const & Location::getLstr() const
{
    return (_lstr);
}

std::string                         const & Location::getLocation() const
{
    return (_location);
}

std::vector<std::string>            const & Location::getMethod() const
{
    return (_method);
}

std::string                         const & Location::getRedirect() const
{
    return (_redirection);
}

std::string                         const & Location::getUpload() const
{
    return (_upload);
}

std::string                         const & Location::getLautoindex() const
{
    return (_lautoindex);
}

std::list<std::string>              const & Location::getIndex() const
{
    return (_lindex);
}

std::map<std::string, std::string>  const & Location::getFastcgi() const
{
    return (_fastcgi_param);
}

std::string                         const & Location::getFastpass() const
{
    return (_fastcgi_pass);
}

ServerPars::ServerPars(): _nb(-1)
{
}

ServerPars::ServerPars(int nb, std::string str): _nb(nb), _str(str), _address(""), _default_name("localhost"), _client_max_body_size(2048)
{
    int         size;
    std::string parsed;

    _nbloc = 0;
    size = 0;
    _error_page.insert(std::pair<int, std::string>(400, "./html/errorPages/400.html"));
    _error_page.insert(std::pair<int, std::string>(403, "./html/errorPages/403.html"));
    _error_page.insert(std::pair<int, std::string>(404, "./html/errorPages/404.html"));
    _error_page.insert(std::pair<int, std::string>(405, "./html/errorPages/405.html"));
    _error_page.insert(std::pair<int, std::string>(413, "./html/errorPages/413.html"));
    _error_page.insert(std::pair<int, std::string>(415, "./html/errorPages/415.html"));
    _error_page.insert(std::pair<int, std::string>(500, "./html/errorPages/500.html"));
    _error_page.insert(std::pair<int, std::string>(501, "./html/errorPages/501.html"));
    while (_str.find("location ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("location ");_str[i] && _str[i] != '}'; i++)
            size++;
        parsed = _str.substr(_str.find("location "), size);
        Location    loc(_nb, _nbloc, parsed);
        _locations.push_back(loc);
        _nbloc++;
        _str.erase(_str.find("location "), size);
    }
    if (_str.find("listen ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("listen ");_str[i] && _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("listen "), size);
        if (checkListen(parsed) == (-1))
        {
            std::cerr << "ERROR .conf listen !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("listen "), size);
    }
    else if (_str.find("listen ") == _str.npos)
        _listen = 80;
    if (_str.find("server_name ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("server_name ");_str[i] && _str[i] != ';'; i++)
        {
            size++;
            if (i + 1 == (int)_str.size())
            {
                std::cerr << "ERROR .conf missing server_name ';' !" << std::endl;
                exit(1);
            }
        }
        parsed = _str.substr(_str.find("server_name "), size);
        if (checkName(parsed) == (-1))
        {
            std::cerr << "ERROR .conf server_name !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("server_name "), size);
    }
    else if (_str.find("server_name ") == _str.npos)
        _server_name.push_back(_default_name);
    while (_str.find("error_page ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("error_page ");_str[i] && _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("error_page "), size);
        if (checkError(parsed) == (-1))
        {
            std::cerr << "ERROR .conf error_page !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("error_page "), size);
    }
    if (_str.find("client_max_body_size ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("client_max_body_size ");_str[i] && _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("client_max_body_size "), size);
        if (checkBody(parsed) == (-1))
        {
            std::cerr << "ERROR .conf body_size !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("client_max_body_size "), size);
    }
    if (_str.find("root ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("root ");_str[i] && _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("root "), size);
        if (checkRoot(parsed) == (-1))
        {
            std::cerr << "ERROR .conf root !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("root "), size);
    }
    else if (_str.find("root ") == _str.npos)
    {
        std::cerr << "ERROR no root in the server !" << std::endl;
        exit(1);
    } 
    if (_str.find("autoindex ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("autoindex ");_str[i] && _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("autoindex "), size);
        if (checkAutoIndex(parsed) == (-1))
        {
            std::cerr << "ERROR .conf autoindex !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("autoindex "), size);
    }
    if (_str.find("index ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("index ");_str[i] && _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("index "), size);
        if (checkIndex(parsed) == (-1))
        {
            std::cerr << "ERROR .conf index !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("index "), size);
    }
    if (_str.find("method ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("method ");_str[i] && _str[i] != ';'; i++)
        {
            size++;
            if (i + 1 == (int)_str.size())
            {
                std::cerr << "ERROR .conf missing method ';' !" << std::endl;
                exit(1);
            }
        }
        parsed = _str.substr(_str.find("method "), size);
        if (checkSmethod(parsed) == (-1))
        {
            std::cerr << "ERROR .conf method !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("method "), size);
    }
    else if (_str.find("method ") == _str.npos)
    {
        std::cerr << "ERROR no method in the server !" << std::endl;
        exit(1);
    } 
    if (_str.find("upload ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("upload ");_str[i] && _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("upload "), size);
        if (checkSupload(parsed) == (-1))
        {
            std::cerr << "ERROR .conf upload !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("upload "), size);
    }
    size = 0;
    while (_str[size])
    {
        if ((_str[size] != ' ') && (_str[size] != ';') && (_str[size] != '{') && (_str[size] != '}') && (_str[size] != '\t') && (_str[size] != '\n') && (_str[size] != '\v') && (_str[size] != '\f') && (_str[size] != '\r'))  
        {
            std::cerr << "ERROR .conf server !" << std::endl;
            exit(1);
        }
        size++;
    }
}

ServerPars::ServerPars(ServerPars const & copy)
{   
    this->_locations = copy._locations;
    this->_fd = copy._fd;
    this->_nb = copy._nb;
    this->_str = copy._str;
    this->_listen = copy._listen;
    this->_address = copy._address;
    this->_server_name = copy._server_name;
    this->_default_name = copy._default_name;
    this->_error_page = copy._error_page;
    this->_client_max_body_size = copy._client_max_body_size;
    this->_root = copy._root;
    this->_autoindex = copy._autoindex;
    this->_index = copy._index;
    this->_smethod = copy._smethod;
    this->_supload = copy._supload;
    *this = copy;
}

ServerPars::~ServerPars()
{
}

ServerPars	& ServerPars::operator=( const ServerPars &other )
{
    if (this != &other)
    {
        this->_locations = other.getLocations();
        this->_fd = other.getFd();
        this->_nb = other.getNb();
        this->_str = other.getStr();
        this->_listen = other.getListen();
        this->_address = other.getAddress();
        this->_server_name = other.getName();
        this->_error_page = other.getError();
        this->_client_max_body_size = other.getBody();
        this->_root = other.getRoot();
        this->_autoindex = other.getAutoindex();
        this->_index = other.getIndex();
        this->_smethod = other.getSmethod();
        this->_supload = other.getSupload();
    }
    return *this;
}

int     ServerPars::checkListen(std::string str)
{
    int size;
    int j;
    
    while (str.find("listen ") != str.npos)
    {
        size = 0;
        for (int i = str.find("listen "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("listen "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if (!(isdigit(str[j])) && (str[j] != '.') && (str[j] != ':'))
        {
            std::cerr << "ERROR listen .conf !" << std::endl;
            exit(1);
        }
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR listen .conf !" << std::endl;
            exit(1);
        }
        _listen += str[j];
        j++;
    }
    if (_listen.find(".") != _listen.npos)
    {
        size = 0;
        while (_listen[size] && _listen[size] != ':')
        {
            _address += _listen[size];
            size++;
        }
        _listen.erase(0, size + 1);
        while (str[j])
        {
            if (!(isdigit(str[j])))
            {
                std::cerr << "ERROR listen .conf !" << std::endl;
                exit(1);
            }
            j++;
        }
    }
    if (_address.compare("127.0.0.1") && (_address != ""))
    {
        std::cerr << "ERROR bad address !" << std::endl;
        exit(1);
    }
    return (0);
}

int     ServerPars::checkName(std::string str)
{
    int         size;
    std::string serv_name;
    
    while (str.find("server_name ") != str.npos)
    {
        size = 0;
        for (int i = str.find("server_name "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("server_name "), size);
    }
    while (str != "")
    {
        size = 0;
        while (str[size] == ' ')
            size++;
        serv_name = str.erase(str.find(" "), size);
        if (serv_name == "")
            break ;
        size = 0;
        while (serv_name[size])
        {
            if ((serv_name[size] == ' ') || (serv_name[size] == '\t') || (serv_name[size] == '\n') || (serv_name[size] == '\v') || (serv_name[size] == '\f') || (serv_name[size] == '\r'))
                break ;
            size++;
        }
        serv_name = serv_name.substr(0, size);
        if (serv_name == "")
        {
            std::cerr << "ERROR server_name .conf !" << std::endl;
            exit(1);
        }
        _server_name.push_back(serv_name);
        str.erase(0, size);
    }
    return (0);
}

int     ServerPars::checkError(std::string str)
{
    int                 size;
    int                 end;
    int                 m;
    int                 ret;
    std::string         error;
    std::string         path;
    std::vector<int>    returns;

    while (str.find("error_page ") != str.npos)
    {
        size = 0;
        for (int i = str.find("error_page "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("error_page "), size + 1);
    }
    size = 0;
    end = 0;
    m = 0;
    while (str[end])
    {
        if ((isdigit(str[end]) == 0) && (str[end] != ' ') && (m == 0))
        {
            size = end ;
            m++;
        }
        end++;
    }
    path = str.substr(size, (end - size));
    for (int i = 0; path[i]; i++)
    {
        if ((path[i] == ' ') || (path[i] == '\t') || (path[i] == '\n') || (path[i] == '\v') || (path[i] == '\f') || (path[i] == '\r'))
        {
            std::cerr << "ERROR eror_page .conf !" << std::endl;
            exit(1);
        }
    }
    end = 0;
    while (str[end] && ((str[end] == ' ') || (str[end] == '\t') || (str[end] == '\n') || (str[end] == '\v') || (str[end] == '\f') || (str[end] == '\r')))
        end++;
    while (str[end] && end < size)
    {
        if ((isdigit(str[end]) == 0) && (str[end] != ' '))
        {
            std::cerr << "ERROR digit eror_page .conf !" << std::endl;
            exit(1);
        }
        if ((str[end] == ' ') || (str[end] == '\t') || (str[end] == '\n') || (str[end] == '\v') || (str[end] == '\f') || (str[end] == '\r'))
        {
            returns.push_back(stoi(error));
            error = "";
        }
        while (str[end] && ((str[end] == ' ') || (str[end] == '\t') || (str[end] == '\n') || (str[end] == '\v') || (str[end] == '\f') || (str[end] == '\r')))
            end++;
        error += str[end];
        end++;
    }
    for (std::vector<int>::iterator it = returns.begin(); it != returns.end(); it++)
    {
        ret = *it;
        _error_page.erase(ret);
        _error_page.insert(std::pair<int, std::string>(ret, path));
    }
    return (0);
}

int     ServerPars::checkBody(std::string str)
{
    std::string parsed;
    int size;
    int j;
    
    while (str.find("client_max_body_size ") != str.npos)
    {
        size = 0;
        for (int i = str.find("client_max_body_size "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("client_max_body_size "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if (!(isdigit(str[j])))
        {
            std::cerr << "ERROR body_size .conf !" << std::endl;
            exit(1);
        }
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR body_size .conf !" << std::endl;
            exit(1);
        }
        parsed += str[j];
        j++;
    }
    str.erase(0, j);
    size = stoi( parsed );
    if (size > 10000)
        perror("client_max_body_size is too big !");
    _client_max_body_size = size;
    return (0);
}

int     ServerPars::checkRoot(std::string str)
{
    int size;
    int j;
    
    while (str.find("root ") != str.npos)
    {
        size = 0;
        for (int i = str.find("root "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("root "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR root .conf !" << std::endl;
            exit(1);
        }
        _root += str[j];
        j++;
    }
    return (0);
}

int     ServerPars::checkIndex(std::string str)
{
    int         size;
    std::string index;
    
    while (str.find("index ") != str.npos)
    {
        size = 0;
        for (int i = str.find("index "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("index "), size);
    }
    while (str != "")
    {
        size = 0;
        while (str[size] == ' ')
            size++;
        index = str.erase(str.find(" "), size);
        if (index == "")
            break ;
        size = 0;
        while (index[size])
        {
            if ((index[size] == ' ') || (index[size] == '\t') || (index[size] == '\n') || (index[size] == '\v') || (index[size] == '\f') || (index[size] == '\r'))
            {
                std::cerr << "ERROR server index .conf !" << std::endl;
                exit(1);
            }
            size++;
        }
        index = index.substr(0, size);
        _index.push_back(index);
        str.erase(0, size);
    }
    return (0);
}

int     ServerPars::checkAutoIndex(std::string str)
{
    int size;
    int j;
    
    while (str.find("autoindex ") != str.npos)
    {
        size = 0;
        for (int i = str.find("autoindex "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("autoindex "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR autoindex .conf !" << std::endl;
            exit(1);
        }
        _autoindex += str[j];
        j++;
    }
    if ((_autoindex.compare("off")) && (_autoindex.compare("on")))
    {
        std::cerr << "ERROR autoindex .conf !" << std::endl;
        exit(1);
    }
    return (0);
}

int     ServerPars::checkSmethod(std::string str)
{
    int         size;
    std::string index;

    while (str.find("method ") != str.npos)
    {
        size = 0;
        for (int i = str.find("method "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("method "), size);
    }
    while (str != "")
    {
        size = 0;
        while (str[size] == ' ')
            size++;
        index = str.erase(str.find(" "), size);
        if (index == "")
            break ;
        size = 0;
        while (index[size])
        {
            if (index[size] == ' ')
                break ;
            size++;
        }
        index = index.substr(0, size);
        _smethod.push_back(index);
        str.erase(0, size);
    }
    for (std::vector<std::string>::iterator it = _smethod.begin(); it != _smethod.end(); it++)
    {
        if (((*it).compare("GET")) && ((*it).compare("POST")) && ((*it).compare("DELETE")))
        {
            std::cerr << "ERROR server method .conf !" << std::endl;
            exit(1);
        }
    }
    return (0);
}

int     ServerPars::checkSupload(std::string str)
{
    int size;
    int j;
    
    while (str.find("upload ") != str.npos)
    {
        size = 0;
        for (int i = str.find("upload "); str[i] != ' '; i++)
            size++;
        str.erase(str.find("upload "), size);
    }
    size = 0;
    while (str[size] == ' ')
        size++;
    j = size;
    while (str[j])
    {
        if ((str[j] == ' ') || (str[j] == '\t') || (str[j] == '\n') || (str[j] == '\v') || (str[j] == '\f') || (str[j] == '\r'))
        {
            std::cerr << "ERROR server upload .conf !" << std::endl;
            exit(1);
        }
        _supload += str[j];
        j++;
    }
    return (0);
}

void    ServerPars::setNb(int & nb)
{
    this->_nb = nb;
}

void    ServerPars::setStr(std::string & _str)
{
    int         size;
    std::string parsed;

    _nbloc = 0;
    size = 0;
    while (_str.find("location ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("location "); _str[i] != '}'; i++)
            size++;
        parsed = _str.substr(_str.find("location "), size);
        Location    loc(_nb, _nbloc, parsed);
        _locations.push_back(loc);
        _nbloc++;
        _str.erase(_str.find("location "), size);
    }
    if (_str.find("listen ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("listen "); _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("listen "), size);
        if (checkListen(parsed) == (-1))
        {
            std::cerr << "ERROR .conf listen !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("listen "), size);
    }
    else if (_str.find("listen ") == _str.npos)
        _listen = 80;
    if (_str.find("server_name ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("server_name "); _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("server_name "), size);

		std::cout << std::endl << "NEW SERVER NAME = [" << parsed << "]" << std::endl << std::endl;

        if (checkName(parsed) == (-1))
        {
            std::cerr << "ERROR .conf server_name !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("server_name "), size);
    }
    else if (_str.find("server_name ") == _str.npos)
        _server_name.push_back(_default_name);
    if (_str.find("error_page ") == _str.npos)
        _error_page.insert(std::pair<int, std::string>(404, "./error_pages/error.html"));
    while (_str.find("error_page ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("error_page "); _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("error_page "), size);
        if (checkError(parsed) == (-1))
        {
            std::cerr << "ERROR .conf error_page !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("error_page "), size);
    }
    if (_str.find("client_max_body_size ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("client_max_body_size "); _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("client_max_body_size "), size);
        if (checkBody(parsed) == (-1))
        {
            std::cerr << "ERROR .conf body_size !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("client_max_body_size "), size);
    }
    if (_str.find("root ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("root "); _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("root "), size);
        if (checkRoot(parsed) == (-1))
        {
            std::cerr << "ERROR .conf root !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("root "), size);
    }
    if (_str.find("autoindex ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("autoindex "); _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("autoindex "), size);
        if (checkAutoIndex(parsed) == (-1))
        {
            std::cerr << "ERROR .conf autoindex !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("autoindex "), size);
    }
    if (_str.find("index ") != _str.npos)
    {
        size = 0;
        for (int i = _str.find("index "); _str[i] != ';'; i++)
            size++;
        parsed = _str.substr(_str.find("index "), size);
        if (checkIndex(parsed) == (-1))
        {
            std::cerr << "ERROR .conf index !" << std::endl;
            exit(1);
        }
        _str.erase(_str.find("index "), size);
    }
    size = 0;
    while (_str[size])
    {
        if ((_str[size] != ' ') && (_str[size] != ';') && (_str[size] != '{') && (_str[size] != '}') && (_str[size] != '\t') && (_str[size] != '\n') && (_str[size] != '\v') && (_str[size] != '\f') && (_str[size] != '\r'))  
        {
            std::cerr << "ERROR .conf server !" << std::endl;
            exit(1);
        }
        size++;
    }
}

void    ServerPars::setFd(int & listenFd) 
{
    this->_fd = listenFd;
}

std::vector<Location>   const & ServerPars::getLocations() const
{
    return (_locations);
}

int    const & ServerPars::getFd() const
{
    return (_fd);
}

int    const & ServerPars::getNb() const
{
    return (_nb);
}

std::string const & ServerPars::getStr() const
{
    return (_str);
}

std::string                 const & ServerPars::getListen() const
{
    return (_listen);
}

std::string                 const & ServerPars::getAddress() const
{
    return (_address);
}

std::list<std::string>      const & ServerPars::getName() const
{
    return (_server_name);
}

std::map<int, std::string>  const & ServerPars::getError() const
{
    return (_error_page);
}

int                         const & ServerPars::getBody() const
{
    return (_client_max_body_size);
}

std::string                 const & ServerPars::getRoot() const
{
    return (_root);
}

std::string                 const & ServerPars::getAutoindex() const
{
    return (_autoindex);
}

std::list<std::string>      const & ServerPars::getIndex() const
{
    return (_index);
}

std::vector<std::string>    const & ServerPars::getSmethod() const
{
    return (_smethod);
}

std::string                 const & ServerPars::getSupload() const
{
    return (_supload);
}