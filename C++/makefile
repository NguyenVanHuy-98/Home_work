CXX =g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET = main
SRC = main.cpp
build-run-clean: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)
	

clean:
	rm -f $(TARGET)

.PHONY: build-run-clean 