# Macros ========================================

CC=gcc
CFLAGS=-Wall -Wextra -ansi -pedantic -g -O2 -Werror -g
LFLAGS=
EXE=cachelist
OBJS=cachelist.o driver.o PRNG.o
ERASE=rm

# TARGETS ========================================

all : $(EXE)
	
$(EXE) : $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(LFLAGS) 

driver.o : driver.c cachelist.h PRNG.h
	$(CC) $(CFLAGS) -c driver.c -o driver.o

cachelist.o : cachelist.c cachelist.h
	$(CC) $(CFLAGS) -c cachelist.c -o cachelist.o

PRNG.o : PRNG.c PRNG.h
	$(CC) $(CFLAGS) -c PRNG.c -o PRNG.o

clean :
	$(ERASE) $(OBJS) $(EXE)

test :