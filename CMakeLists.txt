# SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
# SPDX-License-Identifier: Apache-2.0

# Setup

cmake_minimum_required(VERSION 3.20 FATAL_ERROR) # CI uses `--test-dir`
project(Stitch VERSION 0.1.2 LANGUAGES CXX)

option(STITCH_BUILD_TESTS "Build Stitch tests" OFF)
option(STITCH_BUILD_DOCS "Build Stitch docs" OFF)

# Build types

set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set (CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)

	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Stitch

set(SOURCE_FILES
	"src/ComponentPool.cpp"
	"src/Scene.cpp"
	"src/View.cpp"
)

add_library(Stitch ${SOURCE_FILES})

target_include_directories(Stitch PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(Stitch PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(Stitch PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

# Docs

if (STITCH_BUILD_TESTS)
	add_subdirectory(tests)
endif()

if (STITCH_BUILD_DOCS)
	add_subdirectory(docs)
endif()
