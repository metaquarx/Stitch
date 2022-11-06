// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Stitch/Entity.hpp"
#include "Stitch/Container.hpp"

namespace stch {

template <class T>
struct id {
	using type = T;
};

template <class T>
using id_t = typename id<T>::type;


class Scene {
public:
	Scene();

	EntityID emplace();
	void erase(EntityID id);
	bool is_alive(EntityID id);

	template <typename C, typename... Ps>
	C & emplace(EntityID id, Ps... args);
	template <typename C>
	void erase(EntityID id);

	template <typename... Cs>
	bool all_of(EntityID id) const;
	template <typename... Cs>
	bool any_of(EntityID id) const;

	template <typename C>
	C * get(EntityID id);
	template <typename C>
	const C * get(EntityID id) const;
	template <typename C1, typename C2, typename... Cs>
	std::optional<std::tuple<C1 &, C2 &, Cs &...>> get(EntityID id);

	template <typename... Cs>
	void each(const id_t<std::function<void(Cs &...)>> & callback);

private:
	friend class View;

	std::vector<EntityID> m_recyclable;
	EntityID m_counter;
	std::unordered_map<EntityID, arch::Record> m_entities;

	std::unordered_map<arch::ID, arch::Container, arch::ID::Hash> m_containers;
	std::unordered_map<arch::Type, arch::TypeMap> m_shorthand;
};

}


#include "Stitch/Scene.ipp"
