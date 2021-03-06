// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Stitch/Scene.hpp"

namespace stch {

template <typename... Ts>
std::vector<std::type_index> exclude() {
	return {typeid(Ts)...};
}

template <typename... Cs>
unsigned long Scene::prototype(id_t<std::function<void(EntityID, Cs &...)>> callback) {
	prototypes.push_back([this, &callback]() {
		auto entity = emplace();

		(emplace<Cs>(entity), ...);

		callback(entity, get<Cs>(entity)...);

		return entity;
	});

	return prototypes.size() - 1;
}

template <typename C, typename... Ps>
C &Scene::emplace(EntityID id, Ps... args) {
	auto component_id = std::type_index(typeid(C));

	// create pool if it doesnt exist
	if (pools.find(component_id) == pools.end()) {
		pools.emplace(std::piecewise_construct, std::forward_as_tuple(component_id),
					  std::forward_as_tuple(
						  sizeof(C),
						  [](const std::byte *d) { std::destroy_at(reinterpret_cast<const C *>(d)); },
						  [](std::byte *src, std::byte *dest) {
							  new (dest) C(std::move(*reinterpret_cast<C *>(src)));
						  }));
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

template <typename... Cs>
bool Scene::all_of(EntityID id) const {
	return (... && exists(id, std::type_index(typeid(Cs))));
}

template <typename... Cs>
bool Scene::any_of(EntityID id) const {
	return (... || exists(id, std::type_index(typeid(Cs))));
}

template <typename C>
C &Scene::get(EntityID id) {
	auto component_id = std::type_index(typeid(C));

	auto &pool = pools.at(component_id);
	return *reinterpret_cast<C *>(pool[pool.sparse[id]]);
}

template <typename C1, typename C2, typename... Cs>
std::tuple<C1 &, C2 &, Cs &...> Scene::get(EntityID id) {
	return std::tie(get<C1>(id), get<C2>(id), get<Cs>(id)...);
}

template <typename C>
void Scene::clear() {
	pools.erase(std::type_index(typeid(C)));
}

template <typename C>
void Scene::reserve(std::size_t amount) {
	auto component_id = std::type_index(typeid(C));

	// create pool if it doesnt exist
	if (pools.find(component_id) == pools.end()) {
		pools.emplace(std::piecewise_construct, std::forward_as_tuple(component_id),
					  std::forward_as_tuple(
						  sizeof(C),
						  [](const std::byte *d) { std::destroy_at(reinterpret_cast<const C *>(d)); },
						  [](std::byte *src, std::byte *dest) {
							  new (dest) C(std::move(*reinterpret_cast<C *>(src)));
						  }));
	}

	auto &pool = pools.at(component_id);

	pool.reserve(amount);
}

template <typename... Cs>
View Scene::view(std::vector<std::type_index> exclusions) {
	return View(*this, {std::type_index(typeid(Cs))...}, exclusions);
}

template <typename... Cs>
void Scene::each(const id_t<std::function<void(EntityID, Cs &...)>> &callback) {
	each<Cs...>({}, callback);
}

template <typename... Cs>
void Scene::each(std::vector<std::type_index> exclusions,
				 const id_t<std::function<void(EntityID, Cs &...)>> &callback) {
	auto window = view<Cs...>(exclusions);

	for (auto id : window) {
		callback(id, get<Cs>(id)...);
	}
}

// sorting

namespace priv {

template <typename C>
C &get(unsigned index, ComponentPool &pool) {
	return *reinterpret_cast<C *>(pool[index]);
}

template <typename C>
void swap(unsigned lhs, unsigned rhs, ComponentPool &pool) {
	auto lhs_entity = pool.packed[lhs];
	auto rhs_entity = pool.packed[rhs];

	pool.sparse[lhs_entity] = rhs;
	pool.sparse[rhs_entity] = lhs;
	pool.packed[lhs] = rhs_entity;
	pool.packed[rhs] = lhs_entity;

	C temp = std::move(get<C>(lhs, pool));

	get<C>(lhs, pool) = std::move(get<C>(rhs, pool));
	get<C>(rhs, pool) = std::move(temp);
}

template <typename C>
void quicksort(unsigned first, unsigned last,
			   const std::function<bool(const C &, const C &)> &less_than, ComponentPool &pool) {
	auto size = last - first;

	// already sorted
	if (size <= 1) {
		return;
	}

	// middle pivot
	auto pivot = last;
	--pivot;
	if (size > 2) {
		auto middle = first;
		middle += size / 2;
		swap<C>(middle, pivot, pool);
	}

	// scan and swap
	auto left = first;
	auto right = pivot;
	while (left != right) {
		// look for mismatches
		while (!less_than(get<C>(pivot, pool), get<C>(left, pool)) && left != right) {
			++left;
		}
		while (!less_than(get<C>(right, pool), get<C>(pivot, pool)) && left != right) {
			--right;
		}

		if (left != right) {
			swap<C>(left, right, pool);
		}
	}

	// move pivot back
	if (pivot != left && less_than(get<C>(pivot, pool), get<C>(left, pool))) {
		swap<C>(pivot, left, pool);
	}

	// subdivide
	quicksort(first, left, less_than, pool);
	quicksort(++left, last, less_than, pool);  // left is already sorted
}

}  // namespace priv

template <typename C>
void Scene::sort(const std::function<bool(const C &, const C &)> &less_than) {
	// nothing to sort
	if (pools.find(std::type_index(typeid(C))) == pools.end()) {
		return;
	}

	auto &pool = pools.at(std::type_index(typeid(C)));

	// repack to avoid having to check for num_max
	pool.repack();

	return priv::quicksort(0, pool.capacity(), less_than, pool);
}

}  // namespace stch
