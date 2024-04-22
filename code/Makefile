NAME		:=	webserv
OBJ_DIR		:=	obj
SRC_DIR		:=	srcs
INC_DIR		:=	includes

CXX			:=	c++
CXXFLAGS	:=	-Wall -Werror -Wextra -std=c++98
#CXXFLAGS	+=	-g -fsanitize=address

SRCS_NAME	:=	main.cpp utils.cpp \
Config/Config.cpp Config/ConfigUtils.cpp Config/defaultServSet.cpp Config/getConfigFile.cpp Config/isConfig.cpp Config/servCreate.cpp \
Listen/Listen.cpp \
Connection/Connection.cpp Connection/ConnectionUtils.cpp \
Server/Server.cpp Server/setUp.cpp Server/Run.cpp \
Server/Event/connectTimeout.cpp \
Server/Event/closeConnection.cpp \
Server/Event/prepairingPoll.cpp \
Server/Event/pollWait.cpp \
Server/Event/NewAccept.cpp \
RequestInfo/RequestInfo.cpp \
InputSection/inputSection.cpp \
InputSection/Recv.cpp \
InputSection/Parse.cpp \
Method/Method.cpp Method/Get.cpp Method/Post.cpp Method/Delete.cpp \
Send/Send.cpp \
Error/Error.cpp \
Redirect/Redirect.cpp \
Cgi/CgiExec.cpp Cgi/CgiWait.cpp Cgi/CgiRead.cpp Cgi/CgiWrite.cpp Cgi/CgiParse.cpp \

SRCS		:= $(addprefix $(SRC_DIR)/, $(SRCS_NAME))
OBJECTS		:= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME):	$(OBJECTS)
	$(CXX) $(CXXFLAGS) -I $(INC_DIR) -o $(NAME) $(OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean:	clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all fclean clean re
