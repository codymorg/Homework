# make rebuild -s
# Macros ===============================================================

CC=g++
CFLAGS=-Wall -Wextra -ansi -pedantic -O -Wconversion -std=c++17
OUTDIR=gnu/
EXE=CS260
ERASE=rm
OBJECTS=$(OUTDIR)main.o

# Build target =========================================================

$(EXE) : $(OBJECTS)
	# Compile and link
	$(CC) $(OBJECTS) -o $(EXE)
	
	# Execute
	./$(EXE)

# Objects ==============================================================

$(OUTDIR)main.o : main.cpp sockets.h
	$(CC) $(CFLAGS) -c main.cpp -o $(OUTDIR)main.o
  


# Other targets ========================================================
	
clean :
	$(ERASE) $(EXE) $(OBJECTS)

rebuild :
	$(MAKE) clean
	$(MAKE)

