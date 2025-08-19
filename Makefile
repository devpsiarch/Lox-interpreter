# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -fsanitize=address -g -std=c++17


# Directories
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN := cpplox

# Find all .cpp files in src directory
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
INCS := $(wildcard $(INC_DIR)/*.h) $(INC_DIR)/tool/tool.h


# Replace src/ with build/ and .cpp with .o for object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Default rule
all: $(BIN)

# Link object files to create the binary
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each source file into an object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp  $(INCS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove build files
clean:
	rm -rf $(BUILD_DIR) $(BIN)

# Optional: run the program
# Optional: args is arguments passed by the uses such as this "make run args="something here"
run: $(BIN)
	./$(BIN) $(args) 

.PHONY: all clean run
