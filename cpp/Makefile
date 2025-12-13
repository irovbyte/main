CXX = g++
CXXFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm -lcurl

TARGET = main
SRC = main.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
clean:
	rm -f $(TARGET)