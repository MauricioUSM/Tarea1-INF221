CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-result -Wno-sign-compare -O2

TARGET = sorting

MAIN = sorting.cpp
ALGORITHMS = $(wildcard algorithms/*.cpp)

SOURCES = $(MAIN) $(ALGORITHMS)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

rebuild: clean all