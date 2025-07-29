cmake_minimum_required(VERSION 3.12)
project(MultibodyDynamicsSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Linear algebra backend options
option(USE_ARMADILLO "Use Armadillo as linear algebra backend" ON)
option(BUILD_LINALG_TESTS "Build linear algebra tests" OFF)

# Set backend definitions
if(USE_ARMADILLO)
    add_definitions(-DUSE_ARMADILLO_BACKEND)
endif()

# Try to use Python from PATH, avoiding system directories
execute_process(COMMAND which python OUTPUT_VARIABLE PYTHON_FROM_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
if(PYTHON_FROM_PATH)
    # Check if Python is not from system directories
    string(FIND "${PYTHON_FROM_PATH}" "/opt/homebrew/" IS_HOMEBREW)
    string(FIND "${PYTHON_FROM_PATH}" "/usr/bin/" IS_USR_BIN)
    if(IS_HOMEBREW EQUAL -1 AND IS_USR_BIN EQUAL -1)
        set(Python3_EXECUTABLE ${PYTHON_FROM_PATH})
        message(STATUS "Using Python from PATH (non-system): ${PYTHON_FROM_PATH}")
    else()
        message(STATUS "Found system Python at ${PYTHON_FROM_PATH}, continuing search...")
        unset(PYTHON_FROM_PATH)
    endif()
endif()

# If needed, try devenv command for testing
# if(NOT PYTHON_FROM_PATH)
#     execute_process(
#         COMMAND bash -c "devenv use modmesh && which python"
#         OUTPUT_VARIABLE PYTHON_FROM_DEVENV
#         OUTPUT_STRIP_TRAILING_WHITESPACE
#         ERROR_QUIET
#     )
#     if(PYTHON_FROM_DEVENV)
#         set(Python3_EXECUTABLE ${PYTHON_FROM_DEVENV})
#         message(STATUS "Testing with Python from devenv modmesh: ${PYTHON_FROM_DEVENV}")
#     endif()
# endif()

# Find required packages
if(USE_ARMADILLO)
    find_package(Armadillo REQUIRED)
    if(NOT ARMADILLO_FOUND)
        message(FATAL_ERROR "Armadillo not found. Please install Armadillo or disable USE_ARMADILLO.")
    endif()
    message(STATUS "Using Armadillo ${ARMADILLO_VERSION_STRING}")
endif()
find_program(PYTHON_EXECUTABLE python3)
message(STATUS "use PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}")

# Add pybind11
include(FetchContent)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(pybind11)

# Include directories
include_directories(include)

# Core library sources (excluding main.cpp)
file(GLOB CORE_SOURCES "src/*.cpp")
list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Linear algebra abstraction layer sources
file(GLOB LINALG_SOURCES "src/linalg/*.cpp")

# Create linear algebra abstraction layer library
add_library(LinearAlgebra STATIC ${LINALG_SOURCES})
target_include_directories(LinearAlgebra 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(LinearAlgebra PRIVATE -Wall -g)

# Link linear algebra backend libraries
if(USE_ARMADILLO)
    target_link_libraries(LinearAlgebra PUBLIC ${ARMADILLO_LIBRARIES})
    target_include_directories(LinearAlgebra PUBLIC ${ARMADILLO_INCLUDE_DIRS})
    target_compile_definitions(LinearAlgebra PUBLIC USE_ARMADILLO_BACKEND)
endif()

# Create core library
add_library(MultibodyCore STATIC ${CORE_SOURCES})
target_link_libraries(MultibodyCore PUBLIC LinearAlgebra)
target_include_directories(MultibodyCore 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(MultibodyCore PRIVATE -Wall -g)

# Create standalone executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE MultibodyCore)

# Create Python module
pybind11_add_module(multibody_solver python_bindings/bindings.cpp)
target_link_libraries(multibody_solver PRIVATE MultibodyCore)
target_compile_definitions(multibody_solver PRIVATE VERSION_INFO="dev")

# Optional: Add OpenGL/GLFW for visualization
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 glfw3)
find_package(OpenGL)

if(GLFW3_FOUND AND OPENGL_FOUND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_VISUALIZATION GL_SILENCE_DEPRECATION)
    target_compile_definitions(MultibodyCore PUBLIC ENABLE_VISUALIZATION GL_SILENCE_DEPRECATION)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${OPENGL_LIBRARIES})
    target_link_libraries(MultibodyCore PUBLIC glfw ${OPENGL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW3_INCLUDE_DIRS})
    target_include_directories(MultibodyCore PUBLIC ${GLFW3_INCLUDE_DIRS})
    target_link_directories(${PROJECT_NAME} PRIVATE ${GLFW3_LIBRARY_DIRS})
    target_link_directories(MultibodyCore PUBLIC ${GLFW3_LIBRARY_DIRS})
    
    # Also add to Python module
    target_compile_definitions(multibody_solver PRIVATE ENABLE_VISUALIZATION GL_SILENCE_DEPRECATION)
    target_link_libraries(multibody_solver PRIVATE glfw ${OPENGL_LIBRARIES})
endif()

# Set the output directory for the Python module to be in the current directory
set_target_properties(multibody_solver PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Optional: Build linear algebra tests
if(BUILD_LINALG_TESTS)
    # Find Google Test
    find_package(GTest)
    if(GTest_FOUND OR GTEST_FOUND)
        enable_testing()
        
        # Collect test sources
        file(GLOB TEST_LINALG_SOURCES "tests/test_linalg/*.cpp")
        
        if(TEST_LINALG_SOURCES)
            # Create test executable
            add_executable(linalg_tests ${TEST_LINALG_SOURCES})
            
            # Link libraries
            target_link_libraries(linalg_tests 
                PRIVATE 
                    LinearAlgebra
                    GTest::gtest_main
            )
            
            # Include directories
            target_include_directories(linalg_tests 
                PRIVATE 
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_linalg
            )
            
            # Add tests to CTest
            include(GoogleTest)
            gtest_discover_tests(linalg_tests)
            
            message(STATUS "Linear algebra tests enabled")
        else()
            message(WARNING "No test sources found in tests/test_linalg/")
        endif()
    else()
        message(WARNING "Google Test not found. Linear algebra tests disabled.")
    endif()
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "=== Configuration Summary ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Use Armadillo: ${USE_ARMADILLO}")
if(USE_ARMADILLO AND ARMADILLO_FOUND)
    message(STATUS "Armadillo Version: ${ARMADILLO_VERSION_STRING}")
endif()
message(STATUS "Build Linear Algebra Tests: ${BUILD_LINALG_TESTS}")
message(STATUS "Python Executable: ${PYTHON_EXECUTABLE}")
message(STATUS "===========================")