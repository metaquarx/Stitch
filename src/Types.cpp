// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "Stitch/Types.hpp"

namespace stch::arch {

ID::ID(const Kind &kind) {
	std::hash<std::type_index> hasher;
	m_value = static_cast<uint32_t>(kind.size());
	for (auto type : kind) {
		m_value ^= hasher(type) + 0x9e3779b9 + (m_value << 6) + (m_value >> 2);
	}
}

ID::operator std::uint32_t() const {
	return m_value;
}

bool ID::operator==(ID other) const {
	return m_value == other.m_value;
}

std::size_t ID::Hash::operator()(ID value) const {
	return std::hash<std::uint32_t>()(value);
}

} // namespace stch::arch
