<!-- SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
SPDX-License-Identifier: Apache-2.0 -->

# Tutorial {#mainpage}

Stitch is a type-agnostic [Entity Component System](https://en.wikipedia.org/wiki/Talk:Entity_component_system). This follows the composition-over-inheritance design principle, thereby allowing for greater flexibility when defining entities; just like legos.

Stitch is bitset free - that means that you don't need to specify what components you will use before you use them.

---

## Table of Contents

- [Usage](#usage)
- [Class reference](annotated.html)
- [CMake integration](#cmake)
- [Building manually](#building)

---

## Usage {#usage}

You can get started using Stitch by reading the Scene manual [here](classstch_1_1Scene.html#details).

## CMake integration {#cmake}

The simplest way to integrate Stitch into your project is via [CMake](https://cliutils.gitlab.io/modern-cmake/).

```cmake
include(FetchContent)

FetchContent_Declare(
	Stitch
	GIT_REPOSITORY "https://gitlab.com/metaquarx/stitch"
)

FetchContent_MakeAvailable(Stitch)

# ...

target_link_libraries(my_executable PRIVATE Stitch)
```

### Building manually

If that is not suitable for your project, you can call cmake manually and then copy the artefacts that you need into your project

To build the project,
```
cmake -S . -B build -DSTITCH_BUILD_DOCS=ON -DSTITHC_BUILD_TESTS=ON
cmake --build build
```

To build the documentation (requires doxygen),
```
cmake --build build -t doxygen
```

To run tests,
```
ctest --test-dir build/tests
```
