NAME 			= webserv

OS				= $(shell uname -s)

CXX				= g++

CXXFLAGS 	= -Wall -Werror -Wextra
CXXFLAGS 	+= -Wshadow -Wconversion -pedantic
CXXFLAGS 	+= -g3
CXXFLAGS 	+= -I $(INC)

ifeq ($(OS), Linux)
	CXXFLAGS += -fsanitize=leak
	LDFLAGS += -fsanitize=leak
endif

INC				= include

SRCDIR 		= src

SERVERDIR	= $(addprefix $(SRCDIR)/,server)
SRC_SERVER= Server.cpp
SERVER 		= $(addprefix $(SERVERDIR)/,$(SRC_SERVER))

MAINDIR		= $(addprefix $(SRCDIR)/,main)
SRC_MAIN	= main.cpp
MAIN 			= $(addprefix $(MAINDIR)/,$(SRC_MAIN))

SRC				= $(MAIN) 	\
						$(SERVER)

OBJ 			= $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re:: fclean
re:: all

asan: CXXFLAGS += -fsanitize=address
asan: LDFLAGS += -fsanitize=address
asan: re

.PHONY: all clean fclean re
