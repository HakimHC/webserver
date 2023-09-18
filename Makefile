# Name of the executable
NAME 			= webserv
TESTNAME		= testd
CCGI			= www/cgi-bin/ccgi

SRC_CCGI = www/cgi-bin/CGI.cpp
# Operating System
OS				= $(shell uname -s)

# Compiler
CXX				= g++

# Compiler flags
CXXFLAGS 	= -Wall -Werror -Wextra #-fsanitize=thread
#CXXFLAGS 	+= -Wshadow -Wconversion -pedantic
CXXFLAGS 	+= -g3
CXXFLAGS 	+= $(INC)

# If the OS happens to be Linux, compile with the LeakSanitizer (this flag is not supported on MacOS)
# ifeq ($(OS), Linux)
# 	CXXFLAGS += -fsanitize=leak
# 	LDFLAGS += -fsanitize=leak
# endif

# Header file directory
INC_DIR		= include

INC				= -I $(SERVERDIR)
INC				+= -I $(MAINDIR)
INC				+= -I $(REQUESTDIR)
INC				+= -I $(CLIENTDIR)
INC				+= -I $(UTILSDIR)
INC				+= -I $(HTTPDIR)
INC				+= -I $(INC_DIR)
INC				+= -I $(RESPONSEDIR)
INC				+= -I $(CGIDIR)

# Source code directory
SRCDIR 		= src

# Source code subdirectories and source files
SERVER 		= $(addprefix $(SERVERDIR)/,$(SRC_SERVER))
SERVERDIR	= $(addprefix $(SRCDIR)/,server)
SRC_SERVER= Server.cpp \
						Location.cpp \
						Listener.cpp

CLIENT 		= $(addprefix $(CLIENTDIR)/,$(SRC_CLIENT))
CLIENTDIR	= $(addprefix $(SRCDIR)/,client)
SRC_CLIENT= Client.cpp

REQUEST 		= $(addprefix $(REQUESTDIR)/,$(SRC_REQUEST))
REQUESTDIR	= $(addprefix $(SRCDIR)/,request)
SRC_REQUEST	= Request.cpp

RESPONSE 		= $(addprefix $(RESPONSEDIR)/,$(SRC_RESPONSE))
RESPONSEDIR	= $(addprefix $(SRCDIR)/,response)
SRC_RESPONSE	= Response.cpp

UTILS 		= $(addprefix $(UTILSDIR)/,$(SRC_UTILS))
UTILSDIR	= $(addprefix $(SRCDIR)/,utils)
SRC_UTILS	= crlf.cpp \

HTTP 		= $(addprefix $(HTTPDIR)/,$(SRC_HTTP))
HTTPDIR	= $(addprefix $(SRCDIR)/,http)
SRC_HTTP	= HTTP.cpp \

CGI 		= $(addprefix $(CGIDIR)/,$(SRC_CGI))
CGIDIR	= $(addprefix $(SRCDIR)/,CGI)
SRC_CGI	= CGI.cpp \

MAIN 			= $(addprefix $(MAINDIR)/,$(SRC_MAIN))
MAINDIR		= $(addprefix $(SRCDIR)/,main)
SRC_MAIN	= main.cpp

SRC				= $(MAIN) 	\
						$(SERVER) \
						$(CLIENT) \
						$(REQUEST) \
						$(RESPONSE) \
						$(UTILS) \
						$(HTTP) \
						$(CGI) \


TESTSRC = $(SRC)

#TESTSRC = lowtest.cpp

# Object files
OBJ 			= $(SRC:.cpp=.o)

# Rules
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $@

$(TESTNAME): $(TESTSRC)
	$(CXX) $(CXXFLAGS) $(TESTSRC) $(LDFLAGS) -g3 -o  $@

$(CCGI):
	$(CXX) $(SRC_CCGI) -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)
	$(RM) $(TESTNAME)

test: $(TESTNAME)

re:: fclean
re:: all

# ASan rule (AddressSanitizer)
asan: CXXFLAGS += -fsanitize=address
asan: LDFLAGS += -fsanitize=address
asan: re

cgi: $(CCGI)