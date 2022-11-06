// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "Stitch/Scene.hpp"

#include <limits>

namespace stch {

Scene::Scene() : m_counter(0) {
	arch::Kind empty;
	arch::ID id{empty};

	m_containers.emplace(id, empty);
}

EntityID Scene::emplace() {
	// get id
	EntityID id;
	if (m_recyclable.size()) {
		id = m_recyclable.back();
		m_recyclable.pop_back();
	} else {
		id = m_counter++;
	}

	// add to empty archetype
	m_entities.emplace(id, arch::Record{m_containers.at(arch::ID{{}}), 0});

	return id;
}

void Scene::erase(EntityID id) {
	// clean up
	auto &record = m_entities.at(id);
	arch::Record *end = nullptr;
	for (auto &[eid, record_] : m_entities) {
		if (record_.m_location == record.m_location && record.m_row + 1 == record.m_location->m_storage.m_size) {
			end = &record_;
		}
	}
	record.m_location->erase(record.m_row, *end);
	m_entities.erase(id);

	// recycle id
	constexpr auto max = std::numeric_limits<EntityID>::max();
	constexpr auto next_gen = (max >> (sizeof(EntityID) * 4)) + 1;
	m_recyclable.push_back(id + next_gen);
}

bool Scene::is_alive(EntityID id) {
	return m_entities.count(id);
}

}
