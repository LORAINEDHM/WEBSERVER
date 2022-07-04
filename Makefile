NAME = webserv

SRCS = 	main.cpp \
		utils.cpp \
		ServerConf.cpp \
		ServerPars.cpp \
		webserv.cpp HttpRequest.cpp HttpResponse.cpp Cgi.cpp

OBJS = ${SRCS:.cpp=.o}

CXXFLAGS = -fsanitize=address -Wall -Wextra -Werror -std=c++98
RM = rm -rf
CXX = c++

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean :
	$(RM) $(OBJS)

fclean : clean
	$(RM) $(NAME)

re : fclean all

.PHONY: re fclean clean
