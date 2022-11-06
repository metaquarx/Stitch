// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Stitch/Container.hpp"

namespace stch {

class View {
	struct Iterator {
		Iterator(
			View & parent,
			std::size_t archetypes_idx,
			std::size_t row
		);

		std::pair<arch::Container &, std::size_t> operator*();

		bool operator==(const Iterator & rhs) const;
		bool operator!=(const Iterator & rhs) const;

		Iterator & operator++();
		Iterator operator++(int);

	private:
		View & m_parent;
		std::size_t m_archetypes_idx;
		std::size_t m_row;
	};

public:
	View(class Scene & scene, std::vector<std::type_index> requested);

	Iterator begin();
	Iterator end();

private:
	Scene & m_scene;
	std::vector<arch::ID> m_archetypes;
};

} // namespace stch
