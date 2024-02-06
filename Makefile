FLAGS=-Wall -Werror -Wextra -pg
SECURE=-fsanitize=safe-stack -fstack-protector-all 
LIBS=-lc
# CC=clang
CC=zig cc

REQ = csrc/cweb.c csrc/slayer.c csrc/utils.c 

default : $(REQ)
	$(CC) -o cweb $^ $(FLAGS) $(LIBS) -DDEBUG_PRINT

release : $(REQ)
	$(CC) -o cweb $^ $(FLAGS) $(LIBS) -O3 $(SECURE)

perf :
	python perf/run_httperf.py

zig :
	zig build init	

.PHONY: perf
