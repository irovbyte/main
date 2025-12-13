CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LIBS = -lcurl -lsqlite3

SRCS = src/main.cpp src/memory_sqlite.cpp src/utils.cpp src/web.cpp src/format.cpp src/ai.cpp
OBJS = $(SRCS:.cpp=.o)

BIN = bot
DBFILE = data/memory.db

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)
	@echo "Бот собран. База: $(DBFILE)"

clean:
	rm -f src/*.o $(BIN)