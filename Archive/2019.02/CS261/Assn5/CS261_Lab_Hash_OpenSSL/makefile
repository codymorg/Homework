EXE=hashTest.exe

GPP_SWITCHES=-std=c++11 -Wno-multichar

GPPPARAMS=$(GPP_SWITCHES)

hashTest:
	g++ $(GPPPARAMS) -c main.cpp
	g++ $(GPPPARAMS) -c DigestBuilder.cpp

	g++ -o $(EXE) main.o DigestBuilder.o -lcrypto

clean:
	rm *.o
	rm $(EXE)