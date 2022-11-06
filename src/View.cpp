// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "Stitch/View.hpp"

#include "Stitch/Scene.hpp"

#include <algorithm>

namespace stch {

View::Iterator::Iterator(
	View & parent,
	std::size_t archetypes_idx,
	std::size_t row)
: m_parent(parent)
, m_archetypes_idx(archetypes_idx)
, m_row(row) {
}

std::pair<arch::Container &, std::size_t> View::Iterator::operator*() {
	auto & current_container = m_parent.m_scene.m_containers.at(m_parent.m_archetypes.at(m_archetypes_idx));
	return {current_container, m_row};
}

bool View::Iterator::operator==(const Iterator & rhs) const {
	return (
		&m_parent == &rhs.m_parent &&
		m_archetypes_idx == rhs.m_archetypes_idx &&
		m_row == rhs.m_row
	);
}

bool View::Iterator::operator!=(const Iterator & rhs) const {
	return !(*this == rhs);
}

View::Iterator & View::Iterator::operator++() {
	auto & current_container = m_parent.m_scene.m_containers.at(m_parent.m_archetypes.at(m_archetypes_idx));
	if (current_container.m_storage.m_size == m_row + 1) {
		m_row = 0;
		m_archetypes_idx++;
	} else {
		m_row++;
	}

	return *this;
}

View::Iterator View::Iterator::operator++(int) {
	Iterator temp{*this};
	operator++();
	return temp;
}

View::View(Scene & scene, std::vector<std::type_index> requested)
: m_scene(scene) {
	if (!m_scene.m_shorthand.count(requested.front())) {
		return;
	}

	std::sort(requested.begin(), requested.end());

	const auto & type_map = m_scene.m_shorthand.at(requested.front());
	std::vector<arch::ID> options;
	options.reserve(type_map.size());
	for (auto &[id, column] : type_map) {
		options.push_back(id);
	}

	m_archetypes.reserve(options.size());
	for (auto id : options) {
		if (std::includes(m_scene.m_containers.at(id).m_types.begin(), m_scene.m_containers.at(id).m_types.end(), requested.begin(), requested.end())) {
			if (m_scene.m_containers.at(id).m_storage.m_size) {
				m_archetypes.push_back(id);
			}
		}
	}
}

View::Iterator View::begin() {
	return Iterator(*this, 0, 0);
}

View::Iterator View::end() {
	return Iterator(*this, m_archetypes.size(), 0);
}



} // namespace stch

// ..401 Baz Bar Foo
// ..905 Bar Foo
// ..548 Foo
