// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Stitch/ComponentPool.hpp"
#include "Stitch/View.hpp"

#include <functional>
#include <limits>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>

namespace stch {

/**
 * @brief      An ECS scene that holds all of the entities and their components.
 */
class Scene {
public:
	//////////////////// Entities ////////////////////
	/**
	 * @brief      Create a new entity.
	 *
	 * @return     The new entity's id.
	 */
	EntityID emplace();

	/**
	 * @brief      Erase an entity and its components.
	 *
	 * @param[in]  id    The entity id.
	 *
	 * @warning    Performs no bounds checking on `id`.
	 */
	void erase(EntityID id);

	/**
	 * @brief      Determines if an entity exists.
	 *
	 * @param[in]  id    The entity id.
	 *
	 * @return     True if it exists, false if not.
	 */
	bool exists(EntityID id) const;

	//////////////////// Components ////////////////////

	/**
	 * @brief      Adds a component to an entity.
	 *
	 * @param[in]  id    What entity to add the component to.
	 * @param[in]  args  Optional arguments to pass through to the constructor of the
	 * component.
	 *
	 * @tparam     C     What component to add.
	 *
	 * @return     A reference to the added component.
	 *
	 * @warning    Performs no bounds checking on `id` or `C`.
	 */
	template <typename C, typename... Ps>
	C &emplace(EntityID id, Ps... args);

	/**
	 * @brief      Erase a component from an entity.
	 *
	 * @param[in]  id    What entity to remove the component from.
	 *
	 * @tparam     C     What component to remove.
	 *
	 * @warning    Performs no bounds checking on `id` or `C`.
	 */
	template <typename C>
	void erase(EntityID id);

	/**
	 * @brief      Determines if an entity contains certain components.
	 *
	 * @param[in]  id    The entity id.
	 *
	 * @tparam     Cs     The components to look for.
	 *
	 * @return     True if the entity contains all of the components listed.
	 */
	template <typename... Cs>
	bool all_of(EntityID id) const;

	/**
	 * @brief      Determines if an entity contains certain components.
	 *
	 * @param[in]  id    The entity id.
	 *
	 * @tparam     Cs    The components to look for.
	 *
	 * @return     True if the entity contains any of the components listed.
	 */
	template <typename... Cs>
	bool any_of(EntityID id) const;

	/**
	 * @brief      Gets the component specified.
	 *
	 * @param[in]  id    The entity id.
	 *
	 * @tparam     C     The component to get.
	 *
	 * @return     The component.
	 *
	 * @warning    Performs no bounds checking on `id` or `C`
	 */
	template <typename C>
	C &get(EntityID id);

	//////////////////// Modifiers ////////////////////

	/**
	 * @brief      Optimise the use of memory, by shrinking component pools to their
	 * minimum required.
	 *
	 * @attention  May invalidate any existing reference/pointer to the contents.
	 */
	void repack();

	/**
	 * @brief      Remove all entities and their associated components.
	 */
	void clear();

	/**
	 * @brief      Removes all components of a certain type from all entities, if such a
	 * component exists
	 *
	 * @tparam     C     The component to get rid of.
	 */
	template <typename C>
	void clear();

	/**
	 * @brief      Reserve space inside a component pool;
	 *
	 * This may be useful to avoid reallocations, to avoid invalidating references.
	 *
	 * This function assures a minimum size of `amount`. If this is smaller than the
	 * existing size, this function does nothing.
	 *
	 * @param[in]  amount  The minimum amount of components to store.
	 *
	 * @tparam     C       The components of which to reserve space for.
	 */
	template <typename C>
	void reserve(std::size_t amount);

	//////////////////// Iterable access ////////////////////

	/**
	 * @brief      Construct an iterable View based on the requested components.
	 *
	 * @tparam     Cs    Requested components.
	 */
	template <typename C, typename... Cs>
	View view();

	/**
	 * @brief      Sort a component pool using a custom comparator method
	 *
	 * @param[in]  less_than  User-defined comparator that returns if `left < right`.
	 *
	 * @tparam     C          Component to sort.
	 */
	template <typename C>
	void sort(const std::function<bool(const C &, const C &)> &less_than);

	friend class View;

private:
	/**
	 * @brief      non-templated version of exists<C>(EntityID)
	 */
	bool exists(EntityID id, std::type_index component_id) const;

	/**
	 * @brief      non-templated version of erase<C>(EntityID)
	 */
	void erase(EntityID id, std::type_index component_id);

	// EntityID -> in_use (0/1)
	std::vector<unsigned char> ids;

	// EntityID not in use, shortcut into `ids`
	std::priority_queue<EntityID, std::vector<EntityID>, std::greater<EntityID>> reclaimable_ids;

	// ComponentID -> pool
	std::unordered_map<std::type_index, ComponentPool> pools;
};

}  // namespace stch

#include "Stitch/Scene.ipp"
