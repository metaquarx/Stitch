// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#include "Stitch/ComponentPool.hpp"

#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>

namespace stch {

ComponentPool::ComponentPool(std::size_t type_size,
  std::function<void(const std::byte *)> destructor,
  std::function<void(std::byte *, std::byte *)> move)
  : data(nullptr), dtor(destructor), umove(move), element_size(type_size) {
	reserve(2);
}

ComponentPool::ComponentPool(ComponentPool &&other)
  : sparse(std::move(other.sparse)), packed(std::move(other.packed)),
	reclaimable_packed(std::move(other.reclaimable_packed)), data(other.data),
	dtor(other.dtor), umove(other.umove), element_size(other.element_size) {
	other.data = nullptr;
}

ComponentPool &ComponentPool::operator=(ComponentPool &&other) {
	clear();

	sparse = std::move(other.sparse);
	packed = std::move(other.packed);
	reclaimable_packed = std::move(other.reclaimable_packed);
	data = other.data;
	dtor = other.dtor;
	umove = other.umove;
	element_size = other.element_size;

	other.data = nullptr;

	return *this;
}

ComponentPool::~ComponentPool() {
	clear();

	delete[] data;
}

std::size_t ComponentPool::size() const {
	return capacity() - reclaimable_packed.size();
}

std::size_t ComponentPool::capacity() const {
	return packed.size();
}

void ComponentPool::reserve(std::size_t new_size) {
	if (new_size > capacity()) {
		std::byte *temp = new std::byte[element_size * new_size];

		for (std::size_t i = capacity(); i < new_size; i++) {
			reclaimable_packed.push(i);
		}

		if (data == nullptr) {
			data = temp;
		} else {
			// move from data to temp;
			for (unsigned i = 0; i < packed.size(); i++) {
				if (packed[i] != std::numeric_limits<EntityID>::max()) {
					umove(operator[](i), temp + (i * element_size));
					dtor(operator[](i));
				}
			}

			delete[] data;

			data = temp;
		}

		packed.resize(new_size, std::numeric_limits<EntityID>::max());
	}
}

std::byte *ComponentPool::operator[](std::size_t index) {
	return const_cast<std::byte *>(std::as_const(*this)[index]);
}

const std::byte *ComponentPool::operator[](std::size_t index) const {
	return data + (index * element_size);
}

void ComponentPool::clear() {
	sparse.clear();

	for (std::size_t i = 0; i < packed.size(); i++) {
		if (packed[i] != std::numeric_limits<EntityID>::max()) {
			dtor(operator[](i));
		} else {
			reclaimable_packed.push(i);
		}
	}
}

void ComponentPool::repack() {
	while (sparse.back() == std::numeric_limits<EntityID>::max()) {
		sparse.pop_back();
	}

	auto acc = new std::byte[element_size * size()];
	packed.clear();
	reclaimable_packed = {};

	for (std::size_t i = 0, e = 0; e < sparse.size(); e++) {
		if (sparse[e] != std::numeric_limits<EntityID>::max()) {
			umove(operator[](sparse[e]), acc + (element_size * i));
			dtor(operator[](sparse[e]));

			sparse[e] = i;
			packed.push_back(e);

			i++;
		}
	}

	delete[] data;
	data = acc;
}

std::function<void(const std::byte *)> ComponentPool::get_dtor() {
	return dtor;
}

}  // namespace stch
