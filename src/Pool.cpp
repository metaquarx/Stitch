// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "Stitch/Pool.hpp"

namespace stch::arch {

Pool::Pool(
	PoolInfo & info,
	std::size_t type_size,
	Destructor destructor,
	UninitialisedMove umove,
	Swap swap)
: m_destruct(destructor)
, m_umove(umove)
, m_swap(swap)
, m_storage(&info)
, m_type_size(type_size)
, m_elements(new std::byte[m_type_size * m_storage->m_capacity]) {
}

Pool::Pool(Pool && other)
: m_destruct(other.m_destruct)
, m_umove(other.m_umove)
, m_swap(other.m_swap)
, m_storage(other.m_storage)
, m_type_size(other.m_type_size)
, m_elements(other.m_elements) {
	other.m_elements = nullptr;
}

Pool::~Pool() {
	for (std::size_t i = 0; i < m_storage->m_size; i++) {
		m_destruct(get(i));
	}

	delete[] m_elements;
	m_elements = nullptr;
}

Pool Pool::dupe(PoolInfo & new_info) const {
	return Pool(new_info, m_type_size, m_destruct, m_umove, m_swap);
}

std::byte * Pool::get(std::size_t row) const {
	return m_elements + row * m_type_size;
}

void Pool::erase(std::size_t row) {
	auto * target = get(row);
	auto * end = target;

	if (m_storage->m_size > row + 1) {
		m_swap(get(row), get(m_storage->m_size - 1));
		end = get(m_storage->m_size - 1);
	}

	m_destruct(end);
}

} // namespace stch::arch
