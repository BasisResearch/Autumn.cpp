###############################################################################
# Native build (g++)
###############################################################################
CXX      := g++
CXXFLAGS := -std=c++17 \
            -Iinclude -Iinclude/interpreter -Iinclude/lexer \
            -Iinclude/parser  -Iinclude/interpreter/valuesystem \
            -Iinclude/interpreter/typesystem \
            -O3 -flto \
            -fwhole-program \
            -fno-semantic-interposition \
            -fvisibility=hidden \
            -Wall -Wextra \
            -DAUTUMN_DEV_MODE

###############################################################################
# WebAssembly build (Emscripten / Clang)
###############################################################################
EMCC      := em++
EMCXXFLAGS := -std=c++17 \
              -Iinclude -Iinclude/interpreter -Iinclude/lexer \
              -Iinclude/parser -Iinclude/interpreter/valuesystem \
              -Iinclude/interpreter/typesystem \
              -O3 -flto \
              -s EXPORT_ES6=1 \
              -fwhole-program-vtables \
              -fvisibility=hidden -fvisibility-inlines-hidden \
              -s WASM=1 \
              -s ALLOW_TABLE_GROWTH=1 -s ALLOW_MEMORY_GROWTH=1 \
              --bind \
              -s DYNAMIC_EXECUTION=0 \
              -s ASSERTIONS=1 \
			  -msimd128 \
			  -s NO_DISABLE_EXCEPTION_CATCHING \
              -gsource-map -s LOAD_SOURCE_MAP=0 \
              -s INITIAL_MEMORY=10MB -s MAXIMUM_MEMORY=128MB \
              -DAUTUMN_DEV_MODE

EMLINKFLAGS := -flto \
               -s ALLOW_MEMORY_GROWTH=1 \
               -s MODULARIZE=1 \
               -s 'EXPORT_NAME="createInterpreterModule"' \
               -s ASSERTIONS=1 \
               --bind \
               -gsource-map -s LOAD_SOURCE_MAP=0 \
			   -msimd128 \
			   -s NO_DISABLE_EXCEPTION_CATCHING \
               -s INITIAL_MEMORY=10MB -s MAXIMUM_MEMORY=128MB

# TO enable exception handling in emscripten
# -s DISABLE_EXCEPTION_CATCHING=0  in both emcc and emlink

# Source directories
SRC_DIR := src
TOOLS_DIR := tools
TEST_DIR := testInterpreters
INCLUDE_DIRS := -Iinclude -Iinclude/interpreter -Iinclude/lexer \
                -Iinclude/parser -Iinclude/interpreter/valuesystem \
                -Iinclude/interpreter/typesystem

# Gather all source files
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
INCLUDES := $(shell find include -type d | sed 's/^/-I/')

# Targets
TARGETS := sexp_parser generate_ast lexer parser \
           test_light test_grow interpreter

# Emscripten target
WEB_TARGET := interpreter_web.js
TS_DEF_TARGET := interpreter_web.d.ts

# Output directories
DIST_DIR := dist

# Default target
all: native

.PHONY: native web clean dev web-dev

# Native build
native: $(TARGETS)

# Emscripten build
web: $(WEB_TARGET)

# Build rules for native executables
sexp_parser: $(TOOLS_DIR)/sexp_parser.cpp $(SRC_DIR)/parser/sexpresso.cpp $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/sexp_parser.cpp $(SRC_DIR)/parser/sexpresso.cpp

generate_ast: $(TOOLS_DIR)/generate_ast.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/generate_ast.cpp

lexer: $(TOOLS_DIR)/lexer.cpp $(SRC_DIR)/Token.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/lexer.cpp $(SRC_DIR)/Token.cpp

parser: $(TOOLS_DIR)/parser.cpp $(SRC_DIR)/Token.cpp $(SRC_DIR)/parser/sexpresso.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/parser.cpp $(SRC_DIR)/Token.cpp $(SRC_DIR)/parser/sexpresso.cpp

test_light: $(TEST_DIR)/test_light.cpp $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_DIR)/test_light.cpp $(SOURCES)

test_grow: $(TEST_DIR)/test_grow.cpp $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_DIR)/test_grow.cpp $(SOURCES)

interpreter: $(TOOLS_DIR)/main.cpp $(SOURCES)
	@mkdir -p $(DIST_DIR)
	$(CXX) $(CXXFLAGS) -o $(DIST_DIR)/$@ $(TOOLS_DIR)/main.cpp $(SOURCES)

# Build rule for Emscripten targets - one command generates both .js and .d.ts
$(WEB_TARGET) $(TS_DEF_TARGET): $(TOOLS_DIR)/bindings.cpp $(SOURCES)
	$(EMCC) $(EMCXXFLAGS) -o $(WEB_TARGET) $(TOOLS_DIR)/bindings.cpp $(SOURCES) $(EMLINKFLAGS) --emit-tsd=$(TS_DEF_TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGETS) $(WEB_TARGET) $(TS_DEF_TARGET)
	rm -rf $(DIST_DIR)

# Utility to print sources (optional)
print-sources:
	@echo "SOURCES: $(SOURCES)"

# Development build
dev: CXXFLAGS += $(DEV_FLAGS)
dev: native

# Development web build
web-dev: EMCXXFLAGS += $(EMDEV_FLAGS)
web-dev: web