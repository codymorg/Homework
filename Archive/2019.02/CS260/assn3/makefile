# make rebuild -s
# Macros ===============================================================

CC=g++
CFLAGS=-Wall -Wextra -ansi -pedantic -O -Wconversion -std=c++17

EXE=CS260
ERASE=rm
OBJECTS=  main.o

# Build target =========================================================

  $(EXE) : $(OBJECTS)
	# Compile and link
	$(CC) $(OBJECTS) -o $(EXE)
	

# Objects ==============================================================

  main.o : main.cpp sockets.h
	$(CC) $(CFLAGS) -c main.cpp -o   main.o
  


# Other targets ========================================================
	
clean :
	$(ERASE) $(EXE) $(OBJECTS)

rebuild :
	$(MAKE) clean
	$(MAKE)

