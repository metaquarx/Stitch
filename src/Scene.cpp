// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#include "Stitch/Scene.hpp"

namespace stch {

EntityID Scene::emplace() {
	EntityID out;

	if (reclaimable_ids.size() == 0) {
		ids.push_back(1);
		out = ids.size() - 1;
	} else {
		out = reclaimable_ids.top();
		reclaimable_ids.pop();

		ids[out] = 1;
	}

	return out;
}

EntityID Scene::emplace_prototype(unsigned long prototype_id) {
	return prototypes[prototype_id]();
}

void Scene::erase(EntityID id) {
	for (auto &pool : pools) {
		if (exists(id, pool.first)) {
			erase(id, pool.first);
		}
	}

	ids[id] = 0;

	reclaimable_ids.push(id);
}

bool Scene::exists(EntityID id) const {
	return ids.size() > id && ids[id];
}

void Scene::repack() {
	for (auto it = pools.begin(); it != pools.end();) {
		if (it->second.size() == 0) {
			it = pools.erase(it);
		} else {
			it->second.repack();
			it++;
		}
	}
}

void Scene::clear() {
	for (unsigned id = 0; id < ids.size(); id++) {
		if (ids[id]) {
			erase(id);
		}
	}
}

bool Scene::exists(EntityID id, std::type_index component_id) const {
	// pool doesnt exist
	if (pools.find(component_id) == pools.end()) {
		return false;
	}

	const auto &pool = pools.at(component_id);

	// out of bounds
	if (pool.sparse.size() <= id) {
		return false;
	}

	// check
	return pool.sparse[id] != std::numeric_limits<EntityID>::max();
}

void Scene::erase(EntityID id, std::type_index component_id) {
	auto &pool = pools.at(component_id);
	auto internal = pool.sparse[id];

	pool.get_dtor()(pool[internal]);

	pool.packed[internal] = std::numeric_limits<EntityID>::max();
	pool.reclaimable_packed.push(internal);
	pool.sparse[id] = std::numeric_limits<EntityID>::max();
}

}  // namespace stch
