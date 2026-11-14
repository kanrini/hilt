CXX = g++
HILT_VERSION ?= 0.0.0
CPPFLAGS += -DHILT_BUILD_VERSION=\"$(HILT_VERSION)\"

# Fetch SWORD library configuration via pkg-config
SWORD_CFLAGS := $(shell pkg-config --cflags sword libcurl libarchive)
SWORD_LIBS := $(shell pkg-config --libs sword libcurl libarchive)

# Add the local 'include' directory to the header search path
INC_FLAGS = -Iinclude

# Define compiler flags
# Important: Combine warnings, language standards, library flags, and include paths here.
CXXFLAGS = -Wall -Wextra -std=c++17 $(SWORD_CFLAGS) $(INC_FLAGS)
LDFLAGS = $(SWORD_LIBS)

TARGET = hilt
TEST_TARGET = run_tests

OBJ_DIR = obj
SRC_DIR = src
TEST_DIR = tests

# Define object files
APP_OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/query.o $(OBJ_DIR)/config.o $(OBJ_DIR)/install.o $(OBJ_DIR)/update.o
TEST_OBJS = $(OBJ_DIR)/test_query.o $(OBJ_DIR)/query.o

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
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp include/config.h include/query.h include/install.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/query.o: $(SRC_DIR)/query.cpp include/query.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/config.o: $(SRC_DIR)/config.cpp include/config.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/install.o: $(SRC_DIR)/install.cpp include/install.h include/config.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/update.o: $(SRC_DIR)/update.cpp include/install.h include/config.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/test_query.o: $(TEST_DIR)/test_query.cpp include/query.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

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
