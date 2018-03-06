all:
	g++ -c tinyxml/*.cpp
	g++ tinystr.o tinyxmlparser.o tinyxmlerror.o tinyxml.o -o trabalhocg main.cpp -lGL -lglut

clean:
	rm *.o trabalhocg
