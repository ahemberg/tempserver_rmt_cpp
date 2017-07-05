CC = g++
CFLAGS= -lcurl -lmysqlcppconn -Wall
CSTD= -std=c++11
OBJEDIR = obj/
LOBJDIR = obj/lib/
INCLUDES = lib/
SOURCES= $(wildcard lib/*.cpp)

all: read_temp update_status

read_temp: $(OBJDIR)read_temp.o $(SOURCES:$(INCLUDES)%.cpp=$(LOBJDIR)%.o)
	$(CC) $(CFLAGS) $(CSTD) $^ -o $@

update_status: $(OBJDIR)update_status.o $(SOURCES:$(INCLUDES)%.cpp=$(LOBJDIR)%.o)
	$(CC) $(CFLAGS) $(CSTD) $^ -o $@

$(OBJDIR)read_temp.o: read_temp.cpp
	mkdir -p $(OBJDIR)
	$(CC) -c $(CSTD) $< -o $@

$(OBJDIR)update_status.o: update_status.cpp
	mkdir -p $(OBJDIR)
	$(CC) -c $(CSTD) $< -o $@

$(LOBJDIR)%.o: $(INCLUDES)%.cpp
	mkdir -p $(LOBJDIR)
	$(CC) -c $(CSTD) $< -o $@

clean:
	rm -rf obj/