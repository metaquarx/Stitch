// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "Stitch/Scene.hpp"
#include "catch2/catch_test_macros.hpp"


TEST_CASE("View") {
	stch::Scene registry;

	struct Foo {
		int m_val;
		Foo() : m_val(1234) {}
	};
	struct Bar {};
	struct Baz {};

	SECTION("No items when empty") {
		registry.each<Foo>([](auto &) {
			REQUIRE(false);
		});
	}

	auto id = registry.emplace();
	registry.emplace<Foo>(id);
	registry.emplace<Bar>(id);
	registry.emplace<Baz>(id);

	id = registry.emplace();
	registry.emplace<Foo>(id);
	registry.emplace<Bar>(id);

	SECTION("Looping single item") {
		int count = 0;
		registry.each<Foo>([&](auto & foo) {
			REQUIRE(foo.m_val == 1234);
			count++;
		});
		REQUIRE(count == 2);
	}

	SECTION("Looping multiple items") {
		int count = 0;
		registry.each<Foo, Bar>([&](auto & foo, auto &) {
			REQUIRE(foo.m_val == 1234);
			count++;
		});
		REQUIRE(count == 2);
	}
}
