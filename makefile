
# Compiler ID
CC = g++

# Compiler options // -Wall -Wextra
CXXFLAGS = -std=c++17

# Targets and dependencies
SOURCE = main.cpp Order.cpp TradingClient.cpp MatchingEngine.cpp -lpthread
TARGET = test

build: $(SOURCE)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(SOURCE)
	./$(TARGET)

clean:
	rm -rf $(TARGET)

rebuild: clean build
