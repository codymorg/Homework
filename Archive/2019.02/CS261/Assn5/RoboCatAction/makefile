EXE=RoboCatServer.exe

GPP_SWITCHES=-std=c++11 -Wno-multichar

INCLUDEDIR1=./RoboCat/Inc
INCLUDEDIR2=./RoboCatServer/Inc
INCLUDEDIR3=./SDL/include

GPPPARAMS=$(GPP_SWITCHES) -I$(INCLUDEDIR1) -I$(INCLUDEDIR2) -I$(INCLUDEDIR3)


server:
	g++ $(GPPPARAMS) -c RoboCat/Src/*.cpp
	g++ $(GPPPARAMS) -c RoboCatServer/Src/*.cpp -l crypto
	g++ -o $(EXE) *.o -l crypto

clean:
	rm *.o
	rm $(EXE)
