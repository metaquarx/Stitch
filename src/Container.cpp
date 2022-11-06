// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "Stitch/Container.hpp"
#include "Stitch/Record.hpp"
#include <cassert>

namespace stch::arch {

Container::Container()
: m_id({}) {
}

Container::Container(const Kind & kind)
: m_id(kind)
, m_types(kind)
, m_storage{0, 0} {
}

Container::Container(Container && other)
: m_id(other.m_id)
, m_types(other.m_types)
, m_storage(other.m_storage)
, m_components(std::move(other.m_components))
, m_forward(std::move(other.m_forward))
, m_backward(std::move(other.m_backward)) {
	for (auto & pool : m_components) {
		pool.m_storage = &m_storage;
	}
}

void Container::erase(std::size_t row, Record & end) {
	for (auto &pool : m_components) {
		pool.erase(row);
	}

	if (m_storage.m_size > row + 1) { // swapped

		// update record of row `storage.size` to `row`
		assert(end.m_location == this);
		assert(end.m_row == m_storage.m_size);
		end.m_row = row;
	}
	m_storage.m_size--;

}

std::pair<std::size_t, std::byte *> Container::steal(
	Container & from,
	std::size_t row,
	const std::unordered_map<Type, TypeMap> & shorthand,
	std::optional<Type> remove,
	Record & end) {
	std::size_t target_row = 0;
	if (m_components.size()) {
		// current is non-empty

		if (m_storage.m_capacity == m_storage.m_size) {
			// ran out of slots in pools

			for (auto & pool : m_components) { // each pool
				auto * temp = new std::byte[m_storage.m_capacity * 2 * pool.m_type_size];
				// double pool capacity

				for (std::size_t i = 0; i < m_storage.m_size; i++) {
					// move all existing components to temporary
					auto *old = pool.get(i);
					auto *dest = temp + i * pool.m_type_size;
					pool.m_umove(old, dest);
					pool.m_destruct(old);
				}

				// replace old slots
				delete[] pool.m_elements;
				pool.m_elements = temp;
			}

			m_storage.m_capacity *= 2;
		}

		// pick next row
		target_row = m_storage.m_size;
	}

	std::pair<size_t, std::byte*> ret{target_row, nullptr};

	assert(!from.m_types.size() || from.m_storage.m_size);

	for (std::size_t i = 0; i < m_types.size(); i++) { // each type we do have
		if (shorthand.at(m_types[i]).count(from.m_id)) {
			// steal from source
			auto &pool = m_components[i];
			const auto &column = shorthand.at(m_types[i]).at(from.m_id);
			auto &f_pool = from.m_components[column];

			assert(f_pool.m_type_size == pool.m_type_size);

			// initialise local from source
			pool.m_umove(f_pool.get(row), pool.get(target_row));
			// f_pool.erase(row);
		} else {
			// is new item
			auto &pool = m_components[i];

			assert(!remove);

			ret.second = pool.get(target_row);
		}
	}
	m_storage.m_size++;

	from.erase(row, end);

	return ret;
}

} // namespace stch::arch
