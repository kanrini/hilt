CC = gcc
CXX = g++

# Fetch SWORD library configuration via pkg-config
SWORD_CFLAGS := $(shell pkg-config --cflags sword)
SWORD_LIBS := $(shell pkg-config --libs sword)

# Add the local 'include' directory to the header search path
INC_FLAGS = -Iinclude

# Define compiler flags
# Important: Combine warnings, language standards, library flags, and include paths here.
CFLAGS = -Wall -Wextra -std=c2x $(SWORD_CFLAGS) $(INC_FLAGS)
CXXFLAGS = -Wall -Wextra -std=c++17 $(SWORD_CFLAGS) $(INC_FLAGS)
LDFLAGS = $(SWORD_LIBS)

TARGET = hilt
TEST_TARGET = run_tests

OBJ_DIR = obj
SRC_DIR = src
TEST_DIR = tests

# Define object files
APP_OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/bridge.o $(OBJ_DIR)/config.o
TEST_OBJS = $(OBJ_DIR)/test_bridge.o $(OBJ_DIR)/bridge.o

# Default target
all: $(TARGET)

$(TARGET): $(APP_OBJS)
	$(CXX) $(APP_OBJS) -o $@ $(LDFLAGS)

# Test target
test: $(TEST_TARGET)
	@echo "Running Tests..."
	@./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(TEST_OBJS) -o $@ $(LDFLAGS)

# Compilation rules
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/bridge.o: $(SRC_DIR)/bridge.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/config.o: $(SRC_DIR)/config.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test_bridge.o: $(TEST_DIR)/test_bridge.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

install: $(TARGET)
	@echo "Installing $(TARGET) to $(BINDIR)..."
	@mkdir -p $(BINDIR)
	@cp $(TARGET) $(BINDIR)/$(TARGET)
	@chmod 755 $(BINDIR)/$(TARGET)
	@echo "Installation successful!"

uninstall:
	@echo "Removing $(TARGET) from $(BINDIR)..."
	@rm -f $(BINDIR)/$(TARGET)
	@echo "Uninstallation successful!"

.PHONY: all clean test