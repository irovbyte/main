CXX = g++
CXXFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm

TARGET = main
SRC = main.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
clean:
	rm -f $(TARGET)