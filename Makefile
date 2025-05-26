CXX = g++ # compiler to use
CXXFLAGS = -std=c++17 -Iinclude -Wall -g -pthread -MMD -MP -O2 # compiler flags
# Include header files, enable all compiler warnings, include debug info
# enable multithreading support, enable .d dependency files (automatic rebuilds)
SRC_DIR = src
BUILD_DIR = build

SRC = ${wildcard $(SRC_DIR)/*.cpp}
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))
TARGET = lettuce_server

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: all
	./$(TARGET)

rebuild: clean all
