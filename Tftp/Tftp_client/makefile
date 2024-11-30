# Compiler and flags
CXX := gcc
CXXFLAGS := -Wall -Iinclude        # Basic flags with include path
BUILD_DIR := build
SRC_DIR := src
TARGET := $(BUILD_DIR)/client_exe

# Find all .c files in src directory and its subdirectories
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
# Create corresponding .o file paths in the build directory
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Default rule
all: $(TARGET)

# Rule to create the target executable
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile each .c file into a .o file in the build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build option
debug: CXXFLAGS += -g
debug: all

# Clean up all generated files
clean:
	rm -rf $(BUILD_DIR)

# PHONY targets to avoid conflicts with file names
.PHONY: all clean debug
