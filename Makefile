NAME = woody_woodpacker

NASM = nasm

CC = clang

CFLAGS = -Wall -Wextra -Werror

NASM_FLAG = -f elf64

SRC_DIR = ./srcs/

DIR_OBJ = ./obj/

DIR_INC = ./includes/


FILES = main.c \
		manage_file.c \
		libft.c \
		get.c \
		injection.c

FILES_S = encrypt.s \
		decrypt.s

OBJS = $(patsubst %.c, $(DIR_OBJ)%.o, $(FILES)) \
	$(patsubst %.s, $(DIR_OBJ)%.o, $(FILES_S))

all: $(NAME)

$(NAME): mkobjdir $(FILES) $(FILES_S)
	@ echo "Assembling:"
	@ $(CC) -o $(NAME) $(OBJS)
	@ echo "\033[32mAll done!\033[0m"

mkobjdir:
	@ mkdir -p $(DIR_OBJ)

%.c:
	@ /bin/echo -n "	$(notdir $*).o"
	@ $(CC) -c -o $(DIR_OBJ)$*.o $(SRC_DIR)$@ -I$(DIR_INC)
	@ echo " \033[32mOK\033[0m"

%.s:
	@ /bin/echo -n "	$(notdir $*).o"
	@ $(NASM) $(NASM_FLAG) $(SRC_DIR)$@ -o $(DIR_OBJ)$*.o 
	@ echo " \033[32mOK\033[0m"

debug:
	@ $(CC) -o $(NAME) $(patsubst %.c, $(SRC_DIR)%.c, $(FILES)) -I$(DIR_INC) -g

clean:
	@ /bin/echo -n "Removing object files:"
	@ rm -rf $(DIR_OBJ)
	@ echo " \033[32mdone\033[0m"

fclean: clean
	@ /bin/echo -n "Removing library:"
	@ rm -rf $(NAME)
	@ echo " \033[32mdone\033[0m"

re: fclean all

.PHONY: all mkobjdir clean fclean re