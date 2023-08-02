# Name of the executable
NAME 			= webserv

# Operating System
OS				= $(shell uname -s)

# Compiler
CXX				= g++

# Compiler flags
CXXFLAGS 	= -Wall -Werror -Wextra
CXXFLAGS 	+= -Wshadow -Wconversion -pedantic
CXXFLAGS 	+= -g3
CXXFLAGS 	+= -I $(INC)

# If the OS happens to be Linux, compile with the LeakSanitizer (this flag is not supported on MacOS)
ifeq ($(OS), Linux)
	CXXFLAGS += -fsanitize=leak
	LDFLAGS += -fsanitize=leak
endif

# Header file directory
INC				= include

# Source code directory
SRCDIR 		= src

# Source code subdirectories and source files
SERVERDIR	= $(addprefix $(SRCDIR)/,server)
SRC_SERVER= Server.cpp
SERVER 		= $(addprefix $(SERVERDIR)/,$(SRC_SERVER))

MAINDIR		= $(addprefix $(SRCDIR)/,main)
SRC_MAIN	= main.cpp
MAIN 			= $(addprefix $(MAINDIR)/,$(SRC_MAIN))

SRC				= $(MAIN) 	\
						$(SERVER)

# Object files
OBJ 			= $(SRC:.cpp=.o)

# NGINX container variables for educational / debugging / testing purposes.
IMG_NAME	= nginx-webserv-img

CONF_FILE	= ./container/nginx.conf

CONTAINER	= nginx-webserv

ifeq ($(conf),)
	CONF_FILE = $(conf)
endif

# C++ related rules
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

# NGINX container related rules
build:
	docker build -t $(IMG_NAME) --build-arg CONFIG_FILE=$(CONF_FILE) .

run:
	docker run -p 8080:80 --name $(CONTAINER) $(IMG_NAME)

down:
	docker stop $(CONTAINER)
	docker rm $(CONTAINER)

.PHONY: all clean fclean re build run down
