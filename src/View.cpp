// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#include "Stitch/View.hpp"
#include "Stitch/Scene.hpp"

#include <algorithm>
#include <cassert>

namespace stch {

View::View(Scene &scene_, const std::vector<std::type_index> &requested_)
  : scene(scene_), requested(requested_), request_invalid(false) {
	assert(requested.size() > 0);

	// check if request is valid
	for (auto it = requested.begin(); it != requested.end(); it++) {
		if (scene.pools.find(*it) == scene.pools.end()) {
			request_invalid = true;
		}
	}

	// sort by smallest pool size
	std::sort(
	  requested.begin(), requested.end(), [&](std::type_index lhs, std::type_index rhs) {
		  return scene.pools.at(lhs).size() < scene.pools.at(rhs).size();
	  });
}

View::Iterator View::begin() {
	if (request_invalid) {
		return Iterator(*this, 0);
	}

	unsigned index = 0;

	while (index != scene.pools.at(requested.front()).packed.size() - 1
		   && scene.pools.at(requested.front()).packed[index]
				< std::numeric_limits<EntityID>::max()
		   && !is_valid(index)) {
		index++;
	}

	return Iterator(*this, index);
}

View::Iterator View::end() {
	if (request_invalid) {
		return Iterator(*this, 0);
	}

	unsigned index =
	  static_cast<unsigned>(scene.pools.at(requested.front()).packed.size());

	return Iterator(*this, index);
}

bool View::is_valid(unsigned index) {
	for (auto request : requested) {
		if (!scene.exists(scene.pools.at(requested.front()).packed.at(index), request)) {
			return false;
		}
	}

	return true;
}

View::Iterator::Iterator(View &parent_, unsigned index_)
  : parent(parent_), index(index_) {}

EntityID View::Iterator::operator*() {
	return parent.scene.pools.at(parent.requested.front()).packed[index];
}

bool View::Iterator::operator==(const Iterator &rhs) const {
	return (&parent == &rhs.parent) && (index == rhs.index);
}

bool View::Iterator::operator!=(const Iterator &rhs) const {
	return !(*this == rhs);
}

View::Iterator &View::Iterator::operator++() {
	do {
		index++;
	} while (index < parent.scene.pools.at(parent.requested.front()).packed.size()
			 && !parent.is_valid(index));

	return *this;
}

View::Iterator View::Iterator::operator++(int) {
	auto temp = *this;
	operator++();
	return temp;
}


}  // namespace stch
