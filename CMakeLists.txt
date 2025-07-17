cmake_minimum_required(VERSION 3.5)
project(FunctionalInterpreter)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Emit compile_commands.json for use with clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# If CXX contains clang, set -O3 -flto -fwhole-program-vtables  
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(contain_clang TRUE)
else()
    set(contain_clang FALSE)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(contain_gcc TRUE)
endif()

if(MSVC)                # built-in shortcut for 'CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"'
    set(contain_msvc TRUE)
endif()
message("CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
if(contain_clang)
    message("Contain Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -fwhole-program-vtables")
elseif(contain_gcc)
    message("Contain GCC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto \
        -fno-semantic-interposition -fvisibility=hidden -fvisibility-inlines-hidden")
elseif(contain_msvc)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /flto /GL")
endif()

option(AUTUMN_DEV_MODE "Enable development mode with debug prints" OFF)

if(AUTUMN_DEV_MODE)
    add_definitions(-DAUTUMN_DEV_MODE)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories
include_directories(include include/interpreter include/lexer include/parser include/interpreter/valuesystem include/interpreter/typesystem)

add_executable(sexp_parser tools/sexp_parser.cpp src/parser/sexpresso.cpp)
add_executable(generate_ast tools/generate_ast.cpp)
add_executable(lexer tools/lexer.cpp src/Token.cpp)
add_executable(parser tools/parser.cpp src/Token.cpp src/parser/sexpresso.cpp)

add_library(AutumnLib ${SOURCES})
target_include_directories(AutumnLib PUBLIC
    ${PROJECT_SOURCE_DIR}
)

add_executable(TokenTypeTest
    test_suites/test_token_type.cpp
)

target_link_libraries(TokenTypeTest PRIVATE AutumnLib)

enable_testing()
add_test(NAME TokenTypeTest COMMAND TokenTypeTest)

# Set python executable path
# Check if /opt/homebrew/bin/python exists
if(EXISTS "/opt/homebrew/bin/python")
    set(PYTHON_EXECUTABLE "/opt/homebrew/bin/python")
endif()

message("SOURCES: ${SOURCES}")
# Create the executable
add_executable(interpreter tools/main.cpp ${SOURCES})

find_package(pybind11 REQUIRED)

# Create the Python module
pybind11_add_module(interpreter_module tools/bindings_python.cpp ${SOURCES})

# Try to find Julia
find_package(Julia QUIET)
if(NOT Julia_FOUND)
    # Try to find Julia manually if find_package failed
    find_program(Julia_EXECUTABLE julia DOC "Julia interpreter")
    if(Julia_EXECUTABLE)
        execute_process(
            COMMAND ${Julia_EXECUTABLE} --startup-file=no -e "print(dirname(Sys.BINDIR))"
            OUTPUT_VARIABLE Julia_DIR
        )
        execute_process(
            COMMAND ${Julia_EXECUTABLE} --startup-file=no -e "print(joinpath(Sys.BINDIR, \"../lib\"))"
            OUTPUT_VARIABLE Julia_LIBRARY_DIR
        )
        execute_process(
            COMMAND ${Julia_EXECUTABLE} --startup-file=no -e "print(joinpath(Sys.BINDIR, \"../include/julia\"))"
            OUTPUT_VARIABLE Julia_INCLUDE_DIRS
        )
        
        find_library(Julia_LIBRARY
            NAMES julia libjulia
            PATHS ${Julia_LIBRARY_DIR}
            NO_DEFAULT_PATH
        )

        if(Julia_LIBRARY)
            set(Julia_FOUND TRUE)
            message(STATUS "Found Julia library: ${Julia_LIBRARY}")
            message(STATUS "Julia include dirs: ${Julia_INCLUDE_DIRS}")
        endif()
    endif()
endif()

if(Julia_FOUND)
    message(STATUS "Found Julia: Building Julia bindings")
    find_package(JlCxx QUIET)
    
    if(JlCxx_FOUND)
        # Get Julia package directory
        execute_process(
            COMMAND ${Julia_EXECUTABLE} -e "using Pkg; print(joinpath(first(DEPOT_PATH), \"packages\"))"
            OUTPUT_VARIABLE Julia_PACKAGE_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Create AutumnInterpreter directory if it doesn't exist
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory ${Julia_PACKAGE_DIR}/AutumnInterpreter
        )

        # Add Julia bindings
        add_library(autumn_julia SHARED
            tools/bindings_julia.cpp
        )

        target_link_libraries(autumn_julia
            PRIVATE
            AutumnLib
            JlCxx::cxxwrap_julia
            ${Julia_LIBRARY}
        )

        target_include_directories(autumn_julia
            PRIVATE
            ${Julia_INCLUDE_DIRS}
            ${JlCxx_DIR}/../../../include
        )

        # Copy the library to Julia package directory
        add_custom_command(TARGET autumn_julia POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:autumn_julia> ${Julia_PACKAGE_DIR}/AutumnInterpreter/libautumn_julia${CMAKE_SHARED_LIBRARY_SUFFIX}
        )

        # First, generate a UUID using Julia
        execute_process(
            COMMAND ${Julia_EXECUTABLE} -e "using UUIDs; print(uuid4())"
            OUTPUT_VARIABLE PACKAGE_UUID
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Then use the generated UUID in the Project.toml
        file(WRITE ${Julia_PACKAGE_DIR}/AutumnInterpreter/Project.toml
"name = \"AutumnInterpreter\"
uuid = \"${PACKAGE_UUID}\"
version = \"0.1.0\"

[deps]
CxxWrap = \"1f15a43c-97ca-5a2a-ae31-89f07a497df4\"
")

        file(WRITE ${Julia_PACKAGE_DIR}/AutumnInterpreter/src/AutumnInterpreter.jl
"module AutumnInterpreter

using CxxWrap

# Function to get library path
function get_library_path()
    return joinpath(@__DIR__, \"../libautumn_julia\")
end

# Load the library using the function
@wrapmodule(() -> get_library_path())

function __init__()
    @initcxx
end

export Interpreter,
    run_script,
    step,
    click,
    left,
    right,
    up,
    down,
    get_trigger_state,
    render_all,
    get_environment_string,
    restore_environment,
    tmp_execute_stmt,
    set_verbose,
    get_verbose

end
")
    else()
        message(STATUS "JlCxx not found: Skipping Julia bindings")
    endif()
else()
    message(STATUS "Julia not found: Skipping Julia bindings")
endif()