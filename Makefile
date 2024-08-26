COLOR_RESET  = \033[0m
COLOR_RED    = \033[91m
COLOR_GREEN  = \033[92m
COLOR_YELLOW = \033[93m
COLOR_BOLD   = \033[1m
MAKEFLAGS    += --no-print-directory

NAME         := server_irc

SRC_DIR      := src
OBJ_DIR      := obj

CLASS_DIR    := $(SRC_DIR)/class
SRCS         := $(SRC_DIR)/main.cpp $(CLASS_DIR)/IRC.cpp  $(CLASS_DIR)/Poll.cpp 

INCLUDE      := include
OBJS         := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CFLAGS       := -Wall -Wextra -Werror -g3 
CCFLAGS      := -I $(INCLUDE)
CC           = c++

RM           := rm -rf
DIR_DUP      = mkdir -p $(@D)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(OBJS) -o $(NAME)
	@echo "$(COLOR_GREEN)$(COLOR_BOLD)Compilation finie 👍$(COLOR_RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE)/*
	@$(DIR_DUP)
	$(CC) $(CFLAGS) $(CCFLAGS) -c -o $@ $<

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(OBJ_DIR) $(NAME)
	@echo "$(COLOR_RED)$(COLOR_BOLD)files deleted$(COLOR_RESET)"

re: fclean all

.PHONY: all clean fclean re
