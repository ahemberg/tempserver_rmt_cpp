CFLAGS= -lcurl -lmysqlcppconn
CSTD= -std=c++11

all: read_temp

read_temp: read_temp.o cl_parser.o dbFunctions.o OneWireSensor.o TalkToServer.o read_rpi_board.o
	g++ $(CFLAGS) $(CSTD) read_temp.o cl_parser.o dbFunctions.o OneWireSensor.o TalkToServer.o read_rpi_board.o -o read_temp -Wall

update_status: update_status.o read_rpi_board.o
	g++ $(CSTD) -Wall update_status.o read_rpi_board.o -o update_status

update_status.o: update_status.cpp
	g++ -c $(CSTD) -Wall update_status.cpp

read_temp.o: read_temp.cpp
	g++ -c $(CSTD) read_temp.cpp -Wall

cl_parser.o: lib/cl_parser.cpp
	g++ -c $(CSTD) lib/cl_parser.cpp -Wall

dbFunctions.o: lib/dbFunctions.cpp
	g++ -c $(CSTD) lib/dbFunctions.cpp -Wall

OneWireSensor.o: lib/OneWireSensor.cpp
	g++ -c $(CSTD) lib/OneWireSensor.cpp -Wall

TalkToServer.o: lib/TalkToServer.cpp
	g++ -c $(CSTD) lib/TalkToServer.cpp -Wall

read_rpi_board.o: lib/read_rpi_board.cpp
	g++ -c $(CSTD) lib/read_rpi_board.cpp -Wall

Precompile: lib/pch.h lib/TalkToServer.h lib/OneWireSensor.h lib/cl_parser.h lib/dbFunctions.h
	g++ -std=c++11 lib/pch.h lib/TalkToServer.h lib/OneWireSensor.h lib/cl_parser.h lib/dbFunctions.h

clean:
	rm *.o
