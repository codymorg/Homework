PRG=gnu.exe

GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++11 -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

VALGRIND_OPTIONS=-q --leak-check=full
DIFFLAGS=--strip-trailing-cr -y --suppress-common-lines

OBJECTS0=overlap.cpp
DRIVER0=driver.cpp

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
0 1 2 3 4:
	@echo "running test$@"
	@echo "should run in less than 300 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFFLAGS)
5:
	@echo "running test$@"
	@echo "should run in less than 1000 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFFLAGS)
10:
	@echo "running test$@"
	@echo "should run in less than 50000 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFFLAGS)
clean:
	rm -f *.exe *.o student* difference*
