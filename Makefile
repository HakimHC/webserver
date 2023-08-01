NAME 			= webserv

CXX				= g++

CXXFLAGS 	= -Wall -Werror -Wextra
CXXFLAGS 	+= -Wshadow -Wpedantic
CXXFLAGS 	+= -g3
CXXFLAGS 	+= -I $(INC)

INC				= inc

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
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re:: fclean
re:: all

.PHONY: all clean fclean re
