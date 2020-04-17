CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -Wno-unused-parameter
CXXFILES = matt-daemon.cpp server.cpp Tintin_reporter.cpp
OFILES = $(CXXFILES:%cpp=%o)
LDFLAGS = -Wall -Wextra -Werror
CC = $(CXX)
NAME = matt-daemon

all: $(NAME)

matt-daemon: $(OFILES)

clean:
	$(RM) $(OFILES)

fclean: clean
	$(RM) $(NAME)

re: fclean all
