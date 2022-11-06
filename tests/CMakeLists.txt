# SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
# SPDX-License-Identifier: GPL-3.0-only

include(FetchContent)

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG "v3.1.1"
	GIT_SHALLOW ON
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_executable(Scene "Scene.cpp")
target_link_libraries(Scene PRIVATE Stitch Catch2::Catch2WithMain)
catch_discover_tests(Scene)

add_executable(View "View.cpp")
target_link_libraries(View PRIVATE Stitch Catch2::Catch2WithMain)
catch_discover_tests(View)
