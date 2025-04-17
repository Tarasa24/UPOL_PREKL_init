# Compiler and tools
CXX = g++
FLEX = flex
BISON = bison

# Directories
SRC_DIR = MikroC
BUILD_DIR = build

# Files
LEX_FILE = $(SRC_DIR)/mikroC.l
YACC_FILE = $(SRC_DIR)/mikroC.y
HEADER_FILE = $(SRC_DIR)/mikroC.h
LEX_OUTPUT = $(BUILD_DIR)/lex.yy.c
YACC_OUTPUT = $(BUILD_DIR)/mikroC.tab.c
YACC_HEADER_OUTPUT = $(BUILD_DIR)/mikroC.tab.hpp
OBJ_FILES = $(BUILD_DIR)/mikroC.tab.o $(BUILD_DIR)/lex.yy.o
INTERPRETER = interpreter.cpp
EXECUTABLE = mikroC

# Flags
CXXFLAGS = -I$(BUILD_DIR) -I$(SRC_DIR) -std=c++11 -fpermissive
LDFLAGS = -static

# Default target
all: $(EXECUTABLE)

# Ensure the build directory exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build the parser and lexer
$(BUILD_DIR)/mikroC.tab.c $(BUILD_DIR)/mikroC.tab.hpp: $(YACC_FILE)
	@mkdir -p $(BUILD_DIR)
	$(BISON) -d -o $(BUILD_DIR)/mikroC.tab.c $(YACC_FILE)

$(BUILD_DIR)/lex.yy.c: $(LEX_FILE)
	@mkdir -p $(BUILD_DIR)
	$(FLEX) -o $(BUILD_DIR)/lex.yy.c $(LEX_FILE)

# Compile object files
$(BUILD_DIR)/mikroC.tab.o: $(BUILD_DIR)/mikroC.tab.c $(HEADER_FILE)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/lex.yy.o: $(BUILD_DIR)/lex.yy.c $(HEADER_FILE)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile the interpreter
$(EXECUTABLE): $(OBJ_FILES) $(INTERPRETER)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Clean build directory and executable
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)

.PHONY: all clean