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
# ****************************************************************************

cmake_minimum_required(VERSION 3.0)

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

#MESSAGE( STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE} )

# Define where to put the libraries and binaries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# set(TEST_DATA_PATH ${PROJECT_SOURCE_DIR}/data)

include_directories(include)

# List the include directories...
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)

#install(DIRECTORY include DESTINATION include/casc FILES_MATCHING PATTERN "*.h")

# add a target to generate API documentation with Doxygen
find_package(Doxygen)

message(STATUS "Doxygen ${DOXYGEN_EXECUTABLE}")
if(DOXYGEN_FOUND)
    # Parse Doxyfile.in and replace CMake @macros@.
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    add_custom_target(casc_doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)