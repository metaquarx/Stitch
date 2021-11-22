// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#include "Stitch/ComponentPool.hpp"

#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>

namespace stch {

ComponentPool::ComponentPool(std::size_t type_size, void (*destructor)(const void *))
  : data(nullptr), dtor(destructor), element_size(type_size) {
	reserve(2);
}

ComponentPool::ComponentPool(ComponentPool &&other)
  : sparse(std::move(other.sparse)), packed(std::move(other.packed)),
	reclaimable_packed(std::move(other.reclaimable_packed)), data(other.data),
	dtor(other.dtor), element_size(other.element_size) {
	other.data = nullptr;
}

ComponentPool &ComponentPool::operator=(ComponentPool &&other) {
	clear();

	sparse = std::move(other.sparse);
	packed = std::move(other.packed);
	reclaimable_packed = std::move(other.reclaimable_packed);
	data = other.data;
	dtor = other.dtor;
	element_size = other.element_size;

	other.data = nullptr;

	return *this;
}

ComponentPool::~ComponentPool() {
	clear();

	std::free(data);
}

std::size_t ComponentPool::size() const {
	return capacity() - reclaimable_packed.size();
}

std::size_t ComponentPool::capacity() const {
	return packed.size();
}

void ComponentPool::reserve(std::size_t new_size) {
	if (new_size > capacity()) {
		for (std::size_t i = capacity(); i < new_size; i++) {
			reclaimable_packed.push(i);
		}

		packed.resize(new_size, std::numeric_limits<EntityID>::max());

		data = std::realloc(data, new_size * element_size);
	}
}

void *ComponentPool::operator[](std::size_t index) {
	return const_cast<void *>(std::as_const(*this)[index]);
}

const void *ComponentPool::operator[](std::size_t index) const {
	return static_cast<char *>(data) + (index * element_size);
}

void ComponentPool::clear() {
	sparse.clear();
	reclaimable_packed =
	  std::priority_queue<std::size_t, std::vector<std::size_t>, std::greater<std::size_t>>();

	for (std::size_t i = 0; i < packed.size(); i++) {
		if (packed[i] != std::numeric_limits<EntityID>::max()) {
			dtor(operator[](i));
		}

		reclaimable_packed.push(i);
	}
}

void ComponentPool::repack() {
	while (sparse.back() == std::numeric_limits<EntityID>::max()) {
		sparse.pop_back();
	}

	void *acc = std::malloc(packed.size() * element_size);
	std::size_t acc_i = 0;
	packed.clear();
	reclaimable_packed =
	  std::priority_queue<std::size_t, std::vector<std::size_t>, std::greater<std::size_t>>();

	for (std::size_t i = 0; i < sparse.size(); i++) {
		if (sparse[i] != std::numeric_limits<EntityID>::max()) {
			std::memcpy(static_cast<char *>(acc) + (element_size * acc_i), operator[](sparse[i]),
			  element_size);

			sparse[i] = acc_i;
			packed[acc_i] = i;

			acc_i++;
		}
	}

	std::free(data);
	data = acc;
}

void (*ComponentPool::get_dtor(void) const)(const void *) {
	return dtor;
}

}  // namespace stch
