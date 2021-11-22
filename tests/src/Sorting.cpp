// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#include "idoctest.hpp"

#include "Stitch/Scene.hpp"

#include <iostream>
#include <limits>
#include <random>

struct Component {
	Component(float value_) : value(value_) {}
	float value;
};

TEST_CASE("Sorting") {
	stch::Scene registry;

	std::random_device rdev;
	std::default_random_engine reng(rdev());
	std::uniform_real_distribution<float> rdist(0.f, 10000.f);

	registry.reserve<Component>(1000);

	for (unsigned i = 0; i < 1000; i++) {  // 1000 components should be pretty safe to
										   // avoid starting out in an already sorted
										   // state :)
		auto entity = registry.emplace();
		registry.emplace<Component>(entity, rdist(reng));
	}

	SUBCASE("Not already sorted") {
		bool sorted = true;
		float previous = std::numeric_limits<float>::min();
		for (auto entity : registry.view<Component>()) {
			std::cout << registry.get<Component>(entity).value << ", ";
			if (registry.get<Component>(entity).value < previous) {
				sorted = false;
			}

			previous = registry.get<Component>(entity).value;
		}
		std::cout << "----" << std::endl;
		REQUIRE_FALSE(sorted);
	}

	SUBCASE("Check if sorting works") {
		registry.sort<Component>(
		  [](const auto &lhs, const auto &rhs) { return lhs.value < rhs.value; });

		bool sorted = true;
		float previous = std::numeric_limits<float>::min();
		for (auto entity : registry.view<Component>()) {
			std::cout << registry.get<Component>(entity).value << ", ";
			if (registry.get<Component>(entity).value < previous) {
				sorted = false;
			}
			previous = registry.get<Component>(entity).value;
		}
		std::cout << "----" << std::endl;
		REQUIRE(sorted);
	}
}
