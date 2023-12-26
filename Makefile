FLAGS=-Wall -Werror -Wextra
LIBS=-lc

REQ = src/cweb.c src/slayer.c src/utils.c 

default : $(REQ)
	gcc -o cweb $^ $(FLAGS) $(LIBS)
