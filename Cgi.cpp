#include "Cgi.hpp"

Cgi::Cgi( HttpRequest const & request, ServerPars & server )
{
	char cwd[1024];
	getcwd(cwd, 1024);
	_root = cwd;

	if (request._headers.count("Content-Length"))
		_CONTENT_LENGTH = (request._headers.find("Content-Length")->second);
	if (request.method == "GET")
	{
		_QUERY_STRING = request.queryString;
		_CONTENT_LENGTH = _QUERY_STRING.length();
	}
	_GATEWAY_INTERFACE = std::string("CGI/1.1");
	_PATH_INFO = request.uri;
	_PATH_TRANSLATED = _root + request.uri; 
	_REMOTE_ADDR = std::string("");
	_REMOTE_USER = std::string("");
	_REQUEST_METHOD = request.method;
	_SCRIPT_FILENAME = server.getRoot();
	_SCRIPT_NAME = request.uri;
	_SERVER_NAME = server.getName().front();
	_SERVER_PROTOCOL = "HTTP/1.1";
	_SERVER_PORT = server.getListen();
	_SERVER_SOFTWARE = "webserv";

	_env[0] = std::string("CONTENT_LENGTH=") + _CONTENT_LENGTH;
	_env[1] = std::string("GATEWAY_INTERFACE=") + _GATEWAY_INTERFACE;
	_env[2] = std::string("PATH_INFO=") +_PATH_INFO;
	_env[3] = std::string("PATH_TRANSLATED=") + _PATH_TRANSLATED;
	_env[4] = std::string("QUERY_STRING=") + _QUERY_STRING;
	_env[5] = std::string("REMOTE_ADDR=") + _REMOTE_ADDR;
	_env[6] = std::string("REMOTE_USER=") + _REMOTE_USER;
	_env[7] = std::string("REQUEST_METHOD=") + _REQUEST_METHOD;
	_env[8] = std::string("SCRIPT_FILENAME=") + _SCRIPT_FILENAME;
	_env[9] = std::string("SCRIPT_NAME=") + _SCRIPT_NAME;
	_env[10] = std::string("SERVER_NAME=") + _SERVER_NAME;
	_env[11] = std::string("SERVER_PROTOCOL=") + _SERVER_PROTOCOL;
	_env[12] = std::string("SERVER_PORT=") + _SERVER_PORT;
	_env[13] = std::string("SERVER_SOFTWARE=") + _SERVER_SOFTWARE;
}

int Cgi::run(HttpRequest const & request)
{

	if (request.method == "POST")
	{
		size_t content_length;
		if (request._headers.count("Content-Length"))
		{
			std::istringstream((request._headers.find("Content-Length"))->second) >> content_length;
			if (content_length != request._body.length())
				return 400;
		}
		else
		{
			return 400;
		}
	}
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	int pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	// Child Process
	else if (pid == 0)
	{
		// STDIN_FILENO -> pipe read end.
		// STDOUT_FILENO -> pipe write end.
		if (dup2(pipefd[0], STDIN_FILENO) < 0)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			close(STDIN_FILENO);
			perror("child: dup2");
			exit(EXIT_FAILURE);
		}
		if (dup2(pipefd[1], STDOUT_FILENO) < 0)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			close(STDIN_FILENO);
			perror("child: dup2");
			exit(EXIT_FAILURE);
		}
		close(pipefd[0]);
		close(pipefd[1]);

		// Set arguments execve.
		char *arg[3];
		arg[0] = strdup("/Users/lduhamel/.brew/bin/python3");
		//arg[0] = strdup("/usr/bin/python3");
		//arg[0] = strdup("/usr/bin/python3.8");
		arg[1] = strdup((_root + request.uri).c_str());
		arg[2] = NULL;

		// Create char* environments table from _env of type std::string.
		char *env[16];
		for (size_t i = 0; i < 14; i++)
		{
			env[i] = strdup(_env[i].c_str());
		}
		env[14] = NULL;

		if (execve(arg[0], arg, env))
		{
			perror("execve");

			// If execve fails, we need to free all allocated elements and close all fd's.
			if (pipefd[0] >= 0)
				close(pipefd[0]);
			close(pipefd[1]);

			for (size_t i = 0; i < 3; i++)
				delete arg[i];

			for (size_t i = 0; i < 15; i++)
				delete env[i];
			exit(EXIT_FAILURE);
		}
		else
			exit(0);
	}

	// Parent process
	char buf[1024];
	if (request.method == "POST" && request._body.length() > 0)
	{
		if (write(pipefd[1], request._body.c_str(), request._body.length()) <= 0)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			return 500;
		}
	}
	close(pipefd[1]);

	int status;
	wait(&status);
	if (WEXITSTATUS(status))
		return 500;

	//std::istringstream body_stream;
	int read_bytes;
	bzero(buf, 1024);
	while ((read_bytes = read(pipefd[0], &buf, 1024)) > 0)
	{
		_body += buf;
		bzero(buf, 1024);
	}
	close(pipefd[0]);
	if (read_bytes < 0)
		return 500;
	else
		return 200;
}

std::string const & Cgi::getBody(void)
{
	return _body;
}

Cgi::~Cgi()
{
	return ;
}
