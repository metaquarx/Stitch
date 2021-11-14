// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Stitch/Scene.hpp"

namespace stch {

template <typename C, typename... Ps>
C &Scene::emplace(EntityID id, Ps... args) {
	auto component_id = std::type_index(typeid(C));

	// create pool if it doesnt exist
	if (pools.find(component_id) == pools.end()) {
		pools.emplace(std::piecewise_construct, std::forward_as_tuple(component_id),
		  std::forward_as_tuple(
			sizeof(C), [](const void *d) { static_cast<const C *>(d)->~C(); }));
	}

	auto &pool = pools.at(component_id);

	// resize sparse vector if not big enough
	if (pool.sparse.size() <= id) {
		pool.sparse.resize(id + 1, std::numeric_limits<EntityID>::max());
	}

	// resize internal data pool if needed
	if (pool.reclaimable_packed.size() == 0) {
		pool.reserve(pool.capacity() + 5);
	}

	auto reclaimed = pool.reclaimable_packed.top();
	pool.reclaimable_packed.pop();

	pool.sparse[id] = reclaimed;
	pool.packed[reclaimed] = id;

	C *temp = new (pool[pool.sparse[id]]) C(std::forward<Ps>(args)...);

	return *temp;
}

template <typename C>
void Scene::erase(EntityID id) {
	auto component_id = std::type_index(typeid(C));

	erase(id, component_id);
}

template <typename C>
bool Scene::exists(EntityID id) const {
	auto component_id = std::type_index(typeid(C));

	return exists(id, component_id);
}

template <typename C>
C &Scene::get(EntityID id) {
	auto component_id = std::type_index(typeid(C));

	auto &pool = pools.at(component_id);
	return *static_cast<C *>(pool[pool.sparse[id]]);
}

template <typename C>
void Scene::clear() {
	pools.erase(std::type_index(typeid(C)));
}

template <typename C, typename... Cs>
View Scene::view() {
	return View(*this, {std::type_index(typeid(C)), std::type_index(typeid(Cs))...});
}

}  // namespace stch
