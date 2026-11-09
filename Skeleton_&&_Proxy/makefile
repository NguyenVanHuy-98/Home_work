CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -I.
TARGET = temperature_demo
SRC = main.cpp temperature_provider.cpp temperature_consumer.cpp

all: $(TARGET)

$(TARGET): $(SRC) ara_com.hpp temperature_service.hpp log.hpp temperature_provider.hpp temperature_consumer.hpp
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
