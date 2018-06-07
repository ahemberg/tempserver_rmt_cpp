CC = g++
CFLAGS= -lcurl -lmysqlcppconn -Wall
CSTD= -std=c++11
OBJDIR = obj/
LOBJDIR = obj/lib/
BINDIR = bin/
INCLUDES = lib/
SOURCES= $(wildcard lib/*/*.cpp)

all: read_temp update_status

read_temp: $(OBJDIR)read_temp.o $(SOURCES:$(INCLUDES)%.cpp=$(LOBJDIR)%.o)
	mkdir -p bin
	$(CC) $(CFLAGS) $(CSTD) $^ -o $(BINDIR)$@

update_status: $(OBJDIR)update_status.o $(SOURCES:$(INCLUDES)%.cpp=$(LOBJDIR)%.o)
	mkdir -p bin
	$(CC) $(CFLAGS) $(CSTD) $^ -o $(BINDIR)$@

$(OBJDIR)read_temp.o: read_temp.cpp
	mkdir -p "$(OBJDIR)"
	$(CC) -c $(CSTD) $< -o $@

$(OBJDIR)update_status.o: update_status.cpp
	mkdir -p "$(OBJDIR)"
	$(CC) -c $(CSTD) $< -o $@

$(LOBJDIR)%.o: $(INCLUDES)%.cpp
	#mkdir -p "$(LOBJDIR)"
	find lib/ -type d -links 2 -exec mkdir -p "$(OBJDIR)/{}" \;
	$(CC) -c $(CSTD) $< -o $@

clean:
	rm -rf obj/
