CXX = g++ # compiler to use
CXXFLAGS = -Iinclude -Wall -g # compiler flags
# Include header files, enable all compiler warnings, include debug info
SRC = src/main.cpp src/LettuceServer.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = lettuce_server

all: $(TARGET)

# link object files to create the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
