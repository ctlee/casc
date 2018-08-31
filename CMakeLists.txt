# ***************************************************************************
# This file is part of the Colored Abstract Simplicial Complex library.
# Copyright (C) 2016-2017
# by Christopher Lee, John Moody, Rommie Amaro, J. Andrew McCammon,
#    and Michael Holst
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# ***************************************************************************

cmake_minimum_required(VERSION 3.0)
# Disable in source builds
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

#####################################################################
# Project GAMer
#####################################################################
project(CASC VERSION 1.0.0)

# Require c++14 and standard libraries
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#####################################################################
# Options
#####################################################################
option(BUILD_CASCTESTS "Build the test scripts" OFF)
option(BUILD_CASCEXAMPLES "Build the CASC surface mesh example" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(BUILD_CASCTESTS)
    enable_testing()
endif(BUILD_CASCTESTS)

# Define where to put the libraries and binaries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(include)

# Install directive for the header files
install(DIRECTORY include DESTINATION casc FILES_MATCHING PATTERN "*.h")

# List the include directories...
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "CASC include_dir: ${dir}")
endforeach()

if(BUILD_CASCTESTS)
    add_subdirectory(tests)
endif(BUILD_CASCTESTS)

if(BUILD_CASCEXAMPLES)
    add_subdirectory(examples)
endif(BUILD_CASCEXAMPLES)

#####################################################################
# Target to generate Doxygen documentation
#####################################################################
find_package(Doxygen)
if(DOXYGEN_FOUND)
    message(STATUS "Doxygen Executable: ${DOXYGEN_EXECUTABLE}")
    message(STATUS "Build the documentation using `make docs`")
    # Parse Doxyfile.in and replace CMake @macros@.
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
else()
    message(STATUS "CMake could not find Doxygen. Please install Doxygen or help me find your Doxygen binary to build the documentation!")
endif(DOXYGEN_FOUND)