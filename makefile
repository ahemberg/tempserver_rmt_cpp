CFLAGS= -lcurl -lmysqlcppconn
CSTD= -std=c++11

all: read_temp update_status

read_temp: read_temp.o CommandLineParser.o dbFunctions.o OneWireSensor.o TalkToServer.o SendTempToServer.o read_rpi_board.o RemoteMainDB.o RemoteTempDB.o
	g++ $(CFLAGS) $(CSTD) read_temp.o CommandLineParser.o dbFunctions.o OneWireSensor.o TalkToServer.o SendTempToServer.o RemoteMainDB.o RemoteTempDB.o read_rpi_board.o -o read_temp -Wall

update_status: update_status.o SendStatusToServer.o TalkToServer.o RemoteMainDB.o RemoteStatusDB.o GetBoardStatus.o
	g++ $(CFLAGS) $(CSTD) -Wall update_status.o SendStatusToServer.o TalkToServer.o RemoteStatusDB.o RemoteMainDB.o GetBoardStatus.o -o update_status

update_status.o: update_status.cpp
	g++ -c $(CSTD) -Wall update_status.cpp

read_temp.o: read_temp.cpp
	g++ -c $(CSTD) read_temp.cpp -Wall

CommandLineParser.o: lib/CommandLineParser.cpp
	g++ -c $(CSTD) lib/CommandLineParser.cpp -Wall

dbFunctions.o: lib/dbFunctions.cpp
	g++ -c $(CSTD) lib/dbFunctions.cpp -Wall

RemoteMainDB.o: lib/RemoteMainDB.cpp
	g++ -c $(CSTD) lib/RemoteMainDB.cpp -Wall

RemoteTempDB.o: lib/RemoteTempDB.cpp
	g++ -c $(CSTD) lib/RemoteTempDB.cpp -Wall

RemoteStatusDB.o: lib/RemoteStatusDB.cpp
	g++ -c $(CSTD) lib/RemoteStatusDB.cpp -Wall

OneWireSensor.o: lib/OneWireSensor.cpp
	g++ -c $(CSTD) lib/OneWireSensor.cpp -Wall

TalkToServer.o: lib/TalkToServer.cpp
	g++ -c $(CSTD) lib/TalkToServer.cpp -Wall

SendTempToServer.o: lib/SendTempToServer.cpp
	g++ -c $(CSTD) lib/SendTempToServer.cpp -Wall

SendStatusToServer.o: lib/SendStatusToServer.cpp
	g++ -c $(CSTD) lib/SendStatusToServer.cpp -Wall

GetBoardStatus.o: lib/GetBoardStatus.cpp
	g++ -c $(CSTD) lib/GetBoardStatus.cpp -Wall

read_rpi_board.o: lib/read_rpi_board.cpp
	g++ -c $(CSTD) lib/read_rpi_board.cpp -Wall

Precompile: lib/pch.h lib/TalkToServer.h lib/OneWireSensor.h lib/cl_parser.h lib/dbFunctions.h
	g++ -std=c++11 lib/pch.h lib/TalkToServer.h lib/OneWireSensor.h lib/cl_parser.h lib/dbFunctions.h

clean:
	rm *.o
