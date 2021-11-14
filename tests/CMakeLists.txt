# SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
# SPDX-License-Identifier: Apache-2.0

# FetchContent

include(FetchContent)

# doctest

FetchContent_Declare(
	DocTest
	GIT_REPOSITORY "https://github.com/onqtam/doctest"
	GIT_TAG "2.4.6"
)

FetchContent_MakeAvailable(DocTest)

# CTest

include(CTest)
enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

# Tests

add_library(idoctest SHARED idoctest.cpp)
target_link_libraries(idoctest PUBLIC doctest)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/reports")

macro(RegisterFile SOURCE)
	get_filename_component(NAME ${SOURCE} NAME_WE)

	add_executable(${NAME} "src/${SOURCE}")
	set_target_properties(${NAME} PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
	)

	target_link_libraries(${NAME} PRIVATE idoctest)
	target_link_libraries(${NAME} PRIVATE Stitch)

	target_include_directories(${NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

	add_test(NAME ${NAME} COMMAND ${NAME} --reporters=junit --out=${CMAKE_CURRENT_BINARY_DIR}/reports/${NAME}.xml)
	message(STATUS "Added test ${NAME}")
endmacro()

RegisterFile("Scene.cpp")
RegisterFile("View.cpp")
