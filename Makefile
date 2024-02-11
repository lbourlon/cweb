FLAGS=-Wall -Werror -Wextra -pg -ggdb
SECURE=-fsanitize=safe-stack -fstack-protector-all 
CC=clang

REQ = C/cweb.c C/slayer.c

default : $(REQ)
	$(CC) -o cweb $^ $(FLAGS) $(LIBS) -DDEBUG_PRINT \
		-MJ compile_commands.json

release : $(REQ)
	$(CC) -o cweb $^ $(FLAGS) $(LIBS) -O3 $(SECURE)

perf :
	python perf/run_httperf.py


.PHONY: perf
