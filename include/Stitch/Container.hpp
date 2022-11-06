// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Stitch/Pool.hpp"
#include "Stitch/Types.hpp"

#include <optional>
#include <unordered_map>

namespace stch::arch {

struct Container {
	Container(const Kind & kind);
	Container();
	Container(Container && other);
	~Container() = default;

	Container(const Container &) = delete;
	Container & operator=(Container &&) = delete;
	Container & operator=(const Container &) = delete;

	ID m_id;
	Kind m_types;

	PoolInfo m_storage;
	std::vector<Pool> m_components;

	std::pair<std::size_t, std::byte *> steal(
		Container & from,
		std::size_t row,
		const std::unordered_map<Type, TypeMap> & shorthand,
		std::optional<Type> remove,
		struct Record * end
	);

	void erase(std::size_t row, struct Record * end);

	std::unordered_map<Type, Container &> m_forward;
	std::unordered_map<Type, Container &> m_backward;
};

} // namespace stch::arch
