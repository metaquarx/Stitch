// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Stitch/Scene.hpp"

#include "Stitch/Record.hpp"

#include <bits/utility.h>
#include <iostream>

namespace stch {

template <typename C, typename... Ps>
C &Scene::emplace(EntityID id, Ps... args) {
	auto &current = m_entities.at(id);
	auto target_type = std::type_index(typeid(C));

	if (!current.m_location->m_forward.count(target_type)) {
		// target not cached in current archetype

		auto target_kind = current.m_location->m_types;
		target_kind.insert(
			std::upper_bound(target_kind.begin(), target_kind.end(), target_type),
			target_type
		);
		arch::ID target_kind_id{target_kind};

		if (!m_containers.count(target_kind_id)) {
			// create new archetype
			arch::Container temp{target_kind};
			temp.m_storage.m_size = 0;
			temp.m_storage.m_capacity = 5;

			// transpose component pools
			size_t count = 0;
			for (auto type : temp.m_types) {
				if (type == target_type) {
					// create new pool for current component
					temp.m_components.emplace_back(arch::Pool::create<C>(temp.m_storage));
				} else {
					// transfer old pool layout
					temp.m_components.emplace_back(
						current.m_location->m_components[m_shorthand[type][current.m_location->m_id]].dupe(temp.m_storage)
					);
				}

				// update component lookups
				m_shorthand[type][temp.m_id] = {count++};
			}

			// generate link
			temp.m_backward.emplace(target_type, *current.m_location);

			// add to scene
			m_containers.emplace(temp.m_id, std::move(temp));
		}

		// add to cache
		current.m_location->m_forward.emplace(target_type, m_containers[target_kind_id]);
	}

	auto &target_location = current.m_location->m_forward.at(target_type);

	arch::Record *end = nullptr;
	for (auto &[eid, record] : m_entities) {
		if (record.m_location == &target_location && record.m_row + 1 == target_location.m_storage.m_size) {
			end = &record;
		}
	}
	auto [row, ptr] = target_location.steal(
		*(current.m_location),
		current.m_row,
		m_shorthand,
		std::nullopt,
		*end
	);

	current = arch::Record(target_location, row);
	C *local = new (ptr) C(std::forward<Ps>(args)...);

	return *local;
}

template <typename C>
void Scene::erase(EntityID id) {
	auto &current = m_entities.at(id);
	auto target_type = std::type_index(typeid(C));

	if (!current.m_location->m_backward.count(target_type)) {
		// target not cached in current archetype

		auto target_kind = current.m_location->m_types;
		target_kind.erase(std::find(target_kind.begin(), target_kind.end(), target_type));
		arch::ID target_kind_id{target_kind};

		if (!m_containers.count(target_kind_id)) {
			// create new archetype
			arch::Container temp{target_kind};

			// transpose component pools
			size_t count = 0;
			for (auto type : temp.m_types) {
				temp.m_components.emplace_back(
					current.m_location->m_components[m_shorthand[type][current.m_location->m_id]].dupe(temp.m_storage)
				);

				// update component lookups
				m_shorthand[type][temp.m_id] = {count++};
			}

			// generate link
			temp.m_forward.emplace(target_type, *current.m_location);

			// add to scene
			m_containers.emplace(temp.m_id, std::move(temp));
		}

		// add to cache
		current.m_location->m_backward.emplace(target_type, m_containers[target_kind_id]);
	}

	auto &target_location = current.m_location->m_backward.at(target_type);

	arch::Record *end = nullptr;
	for (auto &[eid, record] : m_entities) {
		if (record.m_location == &target_location && record.m_row + 1 == target_location.m_storage.m_size) {
			end = &record;
		}
	}
	auto [row, ptr] = target_location.steal(
		*(current.m_location),
		current.m_row,
		m_shorthand,
		target_type,
		*end
	);

	current = arch::Record(target_location, row);
}

template <typename... Cs>
bool Scene::all_of(EntityID id) const {
	auto has = [&](std::type_index type) {
		const auto &archetype = *(m_entities.at(id).m_location);

		const auto &archetypes = m_shorthand.at(type);
		return archetypes.count(archetype.m_id);
	};

	return (... && has(std::type_index(typeid(Cs))));
}

template <typename... Cs>
bool Scene::any_of(EntityID id) const {
	auto has = [&](std::type_index type) {
		const auto & record = m_entities.at(id);
		const auto & archetype = *(record.m_location);

		const auto & archetypes = m_shorthand.at(type);
		return archetypes.count(archetype.m_id);
	};

	return (... || has(std::type_index(typeid(Cs))));
}

template <typename C>
const C *Scene::get(EntityID id) const {
	auto type = std::type_index(typeid(C));

	const auto & record = m_entities.at(id);
	const auto & archetype = *(record.m_location);

	auto &archetypes = m_shorthand.at(type);
	if (archetypes.count(archetype.m_id) == 0) {
		return nullptr;
	}

	auto &column = archetypes.at(archetype.m_id);
	return reinterpret_cast<const C *>(
		archetype.m_components[column].get(record.m_row)
	);
}

template <typename C>
C * Scene::get(EntityID id) {
	auto result = static_cast<const Scene *>(this)->get<C>(id);
	return const_cast<C *>(result);
}

template <typename C1, typename C2, typename... Cs>
std::optional<std::tuple<C1 &, C2 &, Cs &...>> Scene::get(EntityID id) {
	auto results = std::forward_as_tuple(get<C1>(id), get<C2>(id), get<Cs>(id)...);

	if (std::apply([](auto &&... i) { return (i && ...); }, results)) {
        return std::apply([](auto &&... i) {
        	return std::tie(*i...);
        }, results);
	}

    return std::nullopt;
}


}
