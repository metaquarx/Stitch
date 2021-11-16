// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#include "idoctest.hpp"

#include "Stitch/Scene.hpp"

struct ComponentOne {};
struct ComponentTwo {};
struct ComponentThree {};

TEST_CASE("View") {
	stch::Scene registry;

	SUBCASE("No items when empty") {
		auto view = registry.view<ComponentOne>();

		REQUIRE(view.begin() == view.end());

		SUBCASE("Multiple components") {
			auto view2 = registry.view<ComponentOne, ComponentTwo>();

			REQUIRE(view2.begin() == view2.end());
		}
	}

	SUBCASE("Items when not empty") {
		auto entity_one = registry.emplace();
		registry.emplace<ComponentOne>(entity_one);

		SUBCASE("With one entity and component") {
			auto view = registry.view<ComponentOne>();
			REQUIRE(*(view.begin()) == entity_one);
			REQUIRE(++view.begin() == view.end());
		}

		SUBCASE("With one entity, multiple components") {
			registry.emplace<ComponentTwo>(entity_one);
			registry.emplace<ComponentThree>(entity_one);

			auto view = registry.view<ComponentTwo>();
			REQUIRE(*(view.begin()) == entity_one);
			REQUIRE(++view.begin() == view.end());
		}

		auto entity_two = registry.emplace();
		auto entity_three = registry.emplace();

		SUBCASE("With multiple entities, some with components") {
			registry.emplace<ComponentTwo>(entity_two);
			registry.emplace<ComponentThree>(entity_two);

			registry.emplace<ComponentOne>(entity_three);
			registry.emplace<ComponentTwo>(entity_three);

			bool seen_entity_one = false;
			bool seen_entity_two = false;
			bool seen_entity_three = false;

			for (auto entity : registry.view<ComponentTwo>()) {
				if (entity == entity_one) seen_entity_one = true;
				if (entity == entity_two) seen_entity_two = true;
				if (entity == entity_three) seen_entity_three = true;
			}

			REQUIRE(seen_entity_one == false);
			REQUIRE(seen_entity_two == true);
			REQUIRE(seen_entity_three == true);
		}

		SUBCASE("With multiple entities, with multiple components") {
			registry.emplace<ComponentTwo>(entity_one);
			registry.emplace<ComponentThree>(entity_one);

			registry.emplace<ComponentOne>(entity_two);
			registry.emplace<ComponentThree>(entity_two);

			registry.emplace<ComponentTwo>(entity_three);
			registry.emplace<ComponentThree>(entity_three);

			bool seen_entity_one = false;
			bool seen_entity_two = false;
			bool seen_entity_three = false;

			for (auto entity : registry.view<ComponentThree, ComponentTwo>()) {
				if (entity == entity_one) seen_entity_one = true;
				if (entity == entity_two) seen_entity_two = true;
				if (entity == entity_three) seen_entity_three = true;
			}

			REQUIRE(seen_entity_one == true);
			REQUIRE(seen_entity_two == false);
			REQUIRE(seen_entity_three == true);
		}
	}
}
