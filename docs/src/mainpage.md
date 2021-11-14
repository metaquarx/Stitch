<!-- SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
SPDX-License-Identifier: Apache-2.0 -->

# Tutorial {#mainpage}

Stitch is a type-agnostic [Entity Component System](https://en.wikipedia.org/wiki/Talk:Entity_component_system). This follows the composition-over-inheritance design principle, thereby allowing for greater flexibility when defining entities; just like legos.

Stitch is bitset free - that means that you don't need to specify what components you will use before you use them.

---

## Table of Contents

- [Usage](#usage)
- [CMake integration](#cmake)
- [Building manually](#building)
- [Class references](annotated.html)

---

## Usage {#usage}

In most cases you'll only need to include the `Scene` header file.

```cpp
#include "Stitch/Scene.hpp"
```

### Handling entities

```cpp
stch::Scene registry;

auto entity = registry.emplace(); // create a new entity, with no components, and store its identifier

registry.erase(entity); // destroy the entity and any components it may have

assert(registry.exists(entity) == false);
```

### Handling components

```cpp
auto entity = registry.emplace();

struct Position {
	float x, y, z;
};

registry.emplace<Position>(entity); // Add the position component to our entity
registry.erase<Position>(entity); // Remove the previously added component

assert(registry.exists(entity));

registry.emplace<Position>(entity, Position{0.f, 1.f, 0.f}); // Optionally pass parameters to the underlying component constructor


auto& position = registry.get<Position>(entity); // Get a reference to the underlying component
assert(position.y == 1.f);

// Everything gets safely destructed at end of scope
```

### Views

Views are used by your systems to iterate over sets of components.

```cpp
void Physics::ForceSystem(float dt) {
	for (auto entity: registry.view<Position, Velocity>()) {

		registry.get<Position>(entity) += registry.get<Velocity>(entity);

	}
}
```

---

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
cmake -S . -B build
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
