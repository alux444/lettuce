CXX = g++ # compiler to use
CXXFLAGS = -std=c++17 -Iinclude -Iexternal -Wall -g -pthread -O2 # compiler flags
# Include header files, enable all compiler warnings, include debug info
# enable multithreading support, enable .d dependency files (automatic rebuilds)
SRC_DIR = src
TESTS_DIR = tests
BUILD_DIR = build

TEST_SRC = tests/test_lettuce.cpp
TEST_OBJ = $(BUILD_DIR)/test_lettuce.o
TEST_TARGET = test_runner

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))
OBJS_NO_MAIN = $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
TARGET = lettuce_server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(TEST_TARGET): $(TEST_OBJ) $(OBJS_NO_MAIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_lettuce.o: $(TEST_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET) $(TEST_OBJ)

run: all
	./$(TARGET)

rebuild: clean all
