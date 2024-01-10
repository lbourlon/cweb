FLAGS=-Wall -Werror -Wextra -pg
LIBS=-lc

REQ = src/cweb.c src/slayer.c src/utils.c 

default : $(REQ)
	gcc -o cweb $^ $(FLAGS) $(LIBS) -DDEBUG_PRINT

release : $(REQ)
	gcc -o cweb $^ $(FLAGS) $(LIBS) -O3

perf :
	python perf/run_httperf.py

.PHONY: perf
