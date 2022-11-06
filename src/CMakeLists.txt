# SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
# SPDX-License-Identifier: GPL-3.0-only

add_library(Stitch
	"Scene.cpp"
	"Container.cpp"
	"Record.cpp"
	"Types.cpp"
	"Pool.cpp")
add_library(Stitch::Stitch ALIAS Stitch)

target_compile_features(Stitch PUBLIC cxx_std_17)
set_target_properties(Stitch PROPERTIES CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "(GNU|CLANG)")
	target_compile_options(Stitch PRIVATE -Wall -Wextra -Wpedantic -Wshallow -Wconversion -Wsign-conversion -Wold-style-cast)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	set_target_properties(Stitch PROPERTIES INTERPROCEDURAL_OPTIMISATION TRUE)
endif()

target_include_directories(Stitch PUBLIC "${Stitch_SOURCE_DIR}/include/")
target_include_directories(Stitch PRIVATE "${Stitch_SOURCE_DIR}/src/")
