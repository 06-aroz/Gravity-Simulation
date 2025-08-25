# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iinclude
LIBS := -framework OpenGL -framework GLUT

# Folders
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN := main

# Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Default rule
all: $(BIN)

# Link
$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIBS)

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if missing
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Running
run: $(BIN)
	./$(BIN)

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN)

.PHONY: all clean run
