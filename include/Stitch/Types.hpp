// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace stch::arch {

using Type = std::type_index;
using Kind = std::vector<Type>;

struct ID {
	ID(const Kind & kind);

	std::uint32_t m_value;

	operator std::uint32_t() const;

	bool operator==(ID other) const;

	struct Hash {
		std::size_t operator()(ID value) const;
	};
};

using TypeMap = std::unordered_map<ID, std::size_t, ID::Hash>;

} // namespace stch::arch
