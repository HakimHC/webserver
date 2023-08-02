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

CLIENT 		= $(addprefix $(CLIENTDIR)/,$(SRC_CLIENT))
SRC_CLIENT= Client.cpp
CLIENTDIR	= $(addprefix $(SRCDIR)/,client)

REQUEST 		= $(addprefix $(REQUESTDIR)/,$(SRC_REQUEST))
SRC_REQUEST	= Request.cpp
REQUESTDIR	= $(addprefix $(SRCDIR)/,request)

MAINDIR		= $(addprefix $(SRCDIR)/,main)
SRC_MAIN	= main.cpp
MAIN 			= $(addprefix $(MAINDIR)/,$(SRC_MAIN))

SRC				= $(MAIN) 	\
						$(SERVER) \
						$(CLIENT) \
						$(REQUEST) \

# Object files
OBJ 			= $(SRC:.cpp=.o)

# NGINX container variables for educational / debugging / testing purposes.
DF_DIR		= nginx
IMG_NAME	= nginx-webserv-img

CONF_FILE	:= nginx.conf

CONTAINER	= nginx-webserv

ifneq ($(conf),)
	CONF_FILE := $(conf)
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
	@echo "CONFIG FILE: " $(CONF_FILE)
	docker build -t $(IMG_NAME) --build-arg CONFIG_FILE=$(CONF_FILE) $(DF_DIR)

run:
	docker run -p 8080:80 --name $(CONTAINER) $(IMG_NAME)

run_detach:
	docker run -d -p 8080:80 --name $(CONTAINER) $(IMG_NAME)

down:
	docker container stop $(CONTAINER)
	docker container rm $(CONTAINER)

.PHONY: all clean fclean re build run down
