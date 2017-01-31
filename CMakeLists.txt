cmake_minimum_required(VERSION 3.6)

project(CASC VERSION 1.0.0)

option(BUILD_TESTS "Build the test scripts" OFF)

if(BUILD_TESTS)
    enable_testing()
endif(BUILD_TESTS)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Print all build flags
message(STATUS "CMAKE_C_FLAGS is: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_C_FLAGS_DEBUG is: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CMAKE_C_FLAGS_RELEASE is: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO is: ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message(STATUS "CMAKE_C_FLAGS_MINSIZEREL is: ${CMAKE_C_FLAGS_MINSIZEREL}")
message(STATUS "CMAKE_CXX_FLAGS is: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG is: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE is: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO is: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "CMAKE_CXX_FLAGS_MINSIZEREL is: ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#MESSAGE( STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE} )

# Define where to put the libraries and binaries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(TEST_DATA_PATH ${PROJECT_SOURCE_DIR}/data)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)

# List the include directories...
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)

message(STATUS "Doxygen ${DOXYGEN_EXECUTABLE}")
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)
