// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#include "idoctest.hpp"

#include "Stitch/Scene.hpp"

#include <memory>

struct Data {
	int *counter;

	Data() {}
	Data(int &watchdog) { set(watchdog); }

	void set(int &watchdog) {
		counter = &watchdog;
		increment();
	}
	void increment() { (*counter)++; }

	~Data() {
		increment();
		counter = nullptr;
	}
};

TEST_CASE("Scene") {
	int counter = 0;
	std::unique_ptr<stch::Scene> p_reg = std::make_unique<stch::Scene>();
	stch::Scene &registry = *p_reg;

	SUBCASE("Adding an entity") {
		auto entity = registry.emplace();
		REQUIRE(registry.exists(entity));

		SUBCASE("Deleting an entity") {
			registry.erase(entity);
			REQUIRE_FALSE(registry.exists(entity));
		}

		SUBCASE("Deleting an entity via clear") {
			registry.clear();
			REQUIRE_FALSE(registry.exists(entity));
		}

		SUBCASE("Adding a component") {
			auto &data = registry.emplace<Data>(entity);
			data.set(counter);

			REQUIRE(counter == 1);
			REQUIRE(registry.exists<Data>(entity));

			SUBCASE("Deleting a component") {
				registry.erase<Data>(entity);
				REQUIRE_FALSE(registry.exists<Data>(entity));
				REQUIRE(counter == 2);
			}

			SUBCASE("Deleting a component via its parent") {
				registry.erase(entity);
				REQUIRE_FALSE(registry.exists(entity));
				REQUIRE(counter == 2);
			}

			SUBCASE("Deleting a component via scene destruction") {
				p_reg.reset();
				REQUIRE(counter == 2);
			}

			SUBCASE("Deleting a component via clear") {
				registry.clear<Data>();
				REQUIRE_FALSE(registry.exists<Data>(entity));
			}

			SUBCASE("Accessing a component via its reference") {
				data.increment();
				REQUIRE(counter == 2);
			}

			SUBCASE("Accessing a component via getter") {
				auto &component = registry.get<Data>(entity);
				component.increment();
				REQUIRE(counter == 2);
			}

			SUBCASE("Reusing a component after it's been removed") {
				registry.erase<Data>(entity);
				counter = 0;

				auto &data2 = registry.emplace<Data>(entity);
				data2.set(counter);

				REQUIRE(counter == 1);
				REQUIRE(registry.exists<Data>(entity));
			}

			SUBCASE("Reusing an entity after it's been removed") {
				registry.erase(entity);
				REQUIRE_FALSE(registry.exists(entity));

				entity = registry.emplace();
				REQUIRE(registry.exists(entity));
				counter = 0;

				auto &data2 = registry.emplace<Data>(entity);
				data2.set(counter);

				REQUIRE(counter == 1);
				REQUIRE(registry.exists<Data>(entity));
			}

			SUBCASE("Repack doesn't impact anything") {
				registry.repack();

				REQUIRE(registry.exists(entity));
				REQUIRE(registry.exists<Data>(entity));

				auto &data2 = registry.get<Data>(entity);
				data2.increment();
				REQUIRE(counter == 2);
			}
		}

		SUBCASE("Adding a component with parameters") {
			auto &data = registry.emplace<Data>(entity, std::ref(counter));

			REQUIRE(counter == 1);
			REQUIRE(&counter == data.counter);
		}
	}

	SUBCASE("Adding many entities and components") {
		for (unsigned i = 0; i < 50; i++) {
			auto entity = registry.emplace();
			registry.emplace<Data>(entity, std::ref(counter));

			entity = registry.emplace();
			auto &data = registry.emplace<Data>(entity);
			data.set(counter);
		}

		registry.clear();

		REQUIRE(counter == 200);
	}

	SUBCASE("Testing non existant IDs for existence") {
		REQUIRE_FALSE(registry.exists(12089123));

		REQUIRE_FALSE(registry.exists<int>(25935992));

		auto entity = registry.emplace();
		REQUIRE_FALSE(registry.exists<Data>(entity));

		registry.emplace<int>(entity);

		REQUIRE_FALSE(registry.exists<int>(1234));
	}
}
