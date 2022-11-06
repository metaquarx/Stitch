# SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
# SPDX-License-Identifier: GPL-3.0-only

# Setup

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(Stitch VERSION 0.1.0 LANGUAGES CXX)

# Subdirectories

add_subdirectory(src)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	include(CTest)
	add_subdirectory(tests)
endif()
