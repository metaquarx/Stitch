// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "Stitch/Scene.hpp"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("Scene") {
	stch::Scene registry;

	SECTION("Non-existent IDs don't exist") {
		REQUIRE_FALSE(registry.is_alive(1));
		REQUIRE_FALSE(registry.is_alive(1234));
	}

	SECTION("Adding an entity") {
		auto entity = registry.emplace();
		REQUIRE(registry.is_alive(entity));

		SECTION("Deleting an entity") {
			registry.erase(entity);
			REQUIRE_FALSE(registry.is_alive(entity));

			SECTION("Readding an entity gives a different generation") {
				auto entity2 = registry.emplace();
				REQUIRE(entity2 != entity);
				REQUIRE(registry.is_alive(entity2));
				REQUIRE_FALSE(registry.is_alive(entity));
			}
		}

		SECTION("Default constructing component") {
			struct Foo {
				int m_value;
				Foo() : m_value(1234) {}
			};
			auto & foo = registry.emplace<Foo>(entity);
			REQUIRE(registry.all_of<Foo>(entity));
			REQUIRE(registry.any_of<Foo>(entity));
			REQUIRE(foo.m_value == 1234);

			SECTION("Getting a component") {
				REQUIRE(registry.get<Foo>(entity) == &foo);
			}

			SECTION("Removing a component") {
				registry.erase<Foo>(entity);
				REQUIRE_FALSE(registry.all_of<Foo>(entity));
				REQUIRE_FALSE(registry.any_of<Foo>(entity));
				REQUIRE(registry.get<Foo>(entity) == nullptr);
			}

			SECTION("Removing a component via its parent entity") {
				registry.erase(entity);

				REQUIRE_THROWS(registry.get<Foo>(entity));
			}
		}

		SECTION("Adding components with parameters") {
			struct Foo {
				int m_value;
				Foo(int value) : m_value(value) {}
			};
			auto & foo = registry.emplace<Foo>(entity, 1234);
			REQUIRE(foo.m_value == 1234);


			SECTION("Getting multiple components at the same time") {
				struct Bar {
					int m_value;
					Bar(int value) : m_value(value) {}
				};
				registry.emplace<Bar>(entity, 1234);

				REQUIRE(registry.all_of<Foo, Bar>(entity));

				auto components = registry.get<Foo, Bar>(entity);
				REQUIRE(components.has_value());
				auto & [foo2, bar] = components.value();
				REQUIRE(foo2.m_value == bar.m_value);

				SECTION("Checking for combinations of components") {
					REQUIRE(registry.all_of<Foo, Bar>(entity));
					REQUIRE(registry.any_of<Foo, Bar>(entity));

					registry.erase<Foo>(entity);
					REQUIRE_FALSE(registry.all_of<Foo, Bar>(entity));
					REQUIRE(registry.any_of<Foo, Bar>(entity));

					registry.erase<Bar>(entity);
					REQUIRE_FALSE(registry.all_of<Foo, Bar>(entity));
					REQUIRE_FALSE(registry.any_of<Foo, Bar>(entity));

					registry.emplace<Bar>(entity, 1);
					REQUIRE_FALSE(registry.all_of<Foo, Bar>(entity));
					REQUIRE(registry.any_of<Foo, Bar>(entity));
				}
			}
		}
	}
}
