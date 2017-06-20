CFLAGS= -lcurl -lmysqlcppconn
CSTD= -std=c++11

all: read_temp

read_temp: main.o cl_parser.o dbFunctions.o OneWireSensor.o TalkToServer.o
	g++ $(CFLAGS) $(CSTD) main.o cl_parser.o dbFunctions.o OneWireSensor.o TalkToServer.o -o read_temp

main.o: main.cpp
	g++ -c $(CSTD) main.cpp

cl_parser.o: lib/cl_parser.cpp
	g++ -c $(CSTD) lib/cl_parser.cpp

dbFunctions.o: lib/dbFunctions.cpp
	g++ -c $(CSTD) lib/dbFunctions.cpp

OneWireSensor.o: lib/OneWireSensor.cpp
	g++ -c $(CSTD) lib/OneWireSensor.cpp

TalkToServer.o: lib/TalkToServer.cpp
	g++ -c $(CSTD) lib/TalkToServer.cpp

Precompile: lib/pch.h lib/TalkToServer.h lib/OneWireSensor.h lib/cl_parser.h lib/dbFunctions.h
	g++ -std=c++11 lib/pch.h lib/TalkToServer.h lib/OneWireSensor.h lib/cl_parser.h lib/dbFunctions.h

clean:
	rm *.o
