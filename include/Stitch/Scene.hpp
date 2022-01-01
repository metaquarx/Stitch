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
 * @brief      Custom implementation of std::type_identity, as we are using C++17, hence not
 * available
 *
 * @tparam     T     type
 */
template <class T>
struct id {
	using type = T;
};

template <class T>
using id_t = typename id<T>::type;

/**
 * @relates Scene
 * @brief      Generate a vector of component types that should be excluded
 *
 * @tparam     Ts    Types to exclude
 *
 * @return     The result of this function can be passed as a parameter to @ref View or @ref each,
 *             to exclude certain components from being iterated
 */
template <typename... Ts>
std::vector<std::type_index> exclude();

/**
 * @brief      An ECS scene that holds all of the entities and their components.
 */
class Scene {
public:
	//////////////////// Prototypes ////////////////////

	/**
	 * @brief      Generates a prototype
	 *
	 * @param[in]  callback  Callback that gets called whenever the prototype is instantiated, with
	 * references to each member.
	 *
	 * @tparam     Cs        Components to automatically add
	 *
	 * @return     An ID unique to the prototype, that you can then use to instantiate it.
	 */
	template <typename... Cs>
	unsigned long prototype(id_t<std::function<void(EntityID, Cs &...)>> callback);

	//////////////////// Entities ////////////////////

	/**
	 * @brief      Create a new entity.
	 *
	 * Create a new entity and return it.
	 *
	 * @return     The new entity's id.
	 */
	EntityID emplace();

	/**
	 * @brief      Create a new entity from a prototype.
	 *
	 * Creates a new entity, which automatically gets assigned all of the components that are in
	 * it's prototype.
	 *
	 * @code
	 * auto prototype = registry.prototype<int, float>([](auto id, auto &i, auto &f) {
	 *     i = 0;
	 *     f = 1.f;
	 * });
	 *
	 * // ...
	 *
	 * auto entity = emplace_prototype(prototype);
	 * @endcode
	 *
	 * @param[in]  prototype_id  The ID to the prototype you wish to instantiate, returned by @ref
	 * prototype.
	 *
	 * @return     The new entity's id.
	 */
	EntityID emplace_prototype(unsigned long prototype_id);

	/**
	 * @brief      Erase an entity and its components.
	 *
	 * @warning    Performs no bounds checking on `id`. You should only call this method if you can
	 * ensure that @ref exists "exists(id) == true" (such as by performing the check on your side,
	 * or using methods such as the @ref View and @ref each methods.
	 *
	 *
	 * @param[in]  id    The entity id.
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
	 * Add a new component to an existing entity, with optional arguments to pass to the constructor.
	 *
	 * @note       The args given are forwarded to the constructor. As such, some initialiser lists
	 * may not implicitly work, and you may have to specify the type to pass. For example, you may
	 * need to pass `MyData(1, 2)` instead of just `{1, 2}`.
	 *
	 * @warning    Performs no bounds checking on `id`. You should only call this method if you can
	 * ensure that @ref exists "exists(id) == true" (such as by performing the check on your side,
	 * or using methods such as the @ref View and @ref each methods.
	 *
	 * @warning    Performs no check to see if the entity already contains the component specified.
	 * You should check against @ref all_of first.
	 *
	 * @param[in]  id    What entity to add the component to.
	 * @param[in]  args  Optional arguments to pass through to the constructor of the component.
	 *
	 * @tparam     C     What component to add.
	 *
	 * @return     A reference to the added component.
	 */
	template <typename C, typename... Ps>
	C &emplace(EntityID id, Ps... args);

	/**
	 * @brief      Erases a component from an entity.
	 *
	 * @warning    Performs no bounds checking on `id`. You should only call this method if you can
	 * ensure that @ref exists "exists(id) == true" (such as by performing the check on your side,
	 * or using methods such as the @ref View and @ref each methods.
	 *
	 * @warning    Performs no check to see if the entity contains the component specified. You
	 * should ensures it exists with a check against @ref all_of first.
	 *
	 * @param[in]  id    What entity to remove from.
	 *
	 * @tparam     C     What component to remove.
	 */
	template <typename C>
	void erase(EntityID id);

	/**
	 * @brief      Checks if an entity has all the given components.
	 *
	 * @warning    Performs no bounds checking on `id`. You should only call this method if you can
	 * ensure that @ref exists "exists(id) == true" (such as by performing the check on your side,
	 * or using methods such as the @ref View and @ref each methods.
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
	 * @brief      Checks if an entity has any of the given components.
	 *
	 * @warning    Performs no bounds checking on `id`. You should only call this method if you can
	 * ensure that @ref exists "exists(id) == true" (such as by performing the check on your side,
	 * or using methods such as the @ref View and @ref each methods.
	 *
	 * @param[in]  id    The entity id.
	 *
	 * @tparam     Cs    The components to look for.
	 *
	 * @return     True if the entity contains atleast one of the components listed.
	 */
	template <typename... Cs>
	bool any_of(EntityID id) const;

	/**
	 * @brief      Get a single component.
	 *
	 * @warning    Performs no bounds checking on `id`. You should only call this method if you can
	 * ensure that @ref exists "exists(id) == true" (such as by performing the check on your side,
	 * or using methods such as the @ref View and @ref each methods.
	 *
	 * @warning    Performs no check to see if the entity contains the component specified. You
	 * should ensures it exists with a check against @ref all_of first.
	 *
	 * @param[in]  id    The entity id.
	 *
	 * @tparam     C     The component to get.
	 *
	 * @return     The a reference to the component requested.
	 */
	template <typename C>
	C &get(EntityID id);

	/**
	 * @brief      Get multiple components.
	 *
	 * The ideal way to use this method is to use structured bindings:
	 *
	 * @code
	 * auto &[position, velocity] = registry.get<Position, Velocity>(entityid);
	 * @endcode
	 *
	 * @warning    Performs no bounds checking on `id`. You should only call this method if you can
	 * ensure that @ref exists "exists(id) == true" (such as by performing the check on your side,
	 * or using methods such as the @ref View and @ref each methods.
	 *
	 * @warning    Performs no check to see if the entity contains the component specified. You
	 * should ensures it exists with a check against @ref all_of first.
	 *
	 * @param[in]  id    The entity id.
	 *
	 * @tparam     Cs    The components to get.
	 *
	 * @return     A tuple containing references to the requested components.
	 */
	template <typename C1, typename C2, typename... Cs>
	std::tuple<C1 &, C2 &, Cs &...> get(EntityID id);

	//////////////////// Modifiers ////////////////////

	/**
	 * @brief      Optimise the use of memory, by shrinking the size of component pools to their
	 * minimum.
	 *
	 * @note       May invalidate any existing references/pointers to any components stored inside.
	 */
	void repack();

	/**
	 * @brief      Remove all entities and all of their associated components.
	 *
	 * All components and entities are removed and destructed.
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
	 * This may be useful to avoid reallocations further down the line, to avoid invalidating
	 * references.
	 *
	 * This function assures a minimum number of @a amount `C` components can be stored inside. If
	 * this is smaller than the existing size, this function does nothing.
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
	 * @code
	 * for (auto entity : registry.view<Position, Velocity>()) {
	 *     auto &[position, velocity] = registry.get<Position, Velocity>(entity);
	 *     position += velocity;
	 * }
	 * @endcode
	 *
	 * For most use cases, the @ref each method may be much more useful.
	 *
	 * @param[in]  exclusions  Component to explicitly exclude while iterating, if any.
	 *
	 * @tparam     Cs          Requested components.
	 */
	template <typename... Cs>
	View view(std::vector<std::type_index> exclusions = {});

	/**
	 * @brief      Abstraction over the view function, that lets you iterate using a callback.
	 *
	 * This method should be preferred over the @ref view function, as it means you no longer need
	 * to manually call @ref get on each component that you need to loop over. Instead, you can
	 * simply pass a callback that will automatically be filled.
	 *
	 * @code
	 * registry.each<Position, Velocity>([](auto id, auto &pos, auto &vel) {
	 *     pos += vel;
	 * });
	 * @endcode
	 *
	 * @param[in]  callback  The callback function.
	 *
	 * @tparam     Cs        Requested components.
	 */
	template <typename... Cs>
	void each(const id_t<std::function<void(EntityID, Cs &...)>> &callback);

	/**
	 * @brief      Overload of each function, that lets you exclude certain components.
	 *
	 * @code
	 * registry.each<Position, Velocity>(stch::exclude<DisablePhysics>(),
	 *                                   [](auto id, auto &pos, auto &vel) {
	 *     pos += vel;
	 * });
	 * @endcode
	 *
	 * @param[in]  exclusions  The exclusions
	 * @param[in]  callback    The callback
	 *
	 * @tparam     Cs          Requested components
	 */
	template <typename... Cs>
	void each(std::vector<std::type_index> exclusions,
			  const id_t<std::function<void(EntityID, Cs &...)>> &callback);

	/**
	 * @brief      Sort a component pool using a custom comparator method.
	 *
	 * @code
	 * registry.sort<Component>([](const auto &lhs, const auto &rhs) {
	 *     return lhs.value < rhs.value;
	 * });
	 * @endcode
	 *
	 * @param[in]  less_than  User-defined comparator that returns if `lhs < rhs`.
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

	std::vector<std::function<EntityID()>> prototypes;
};

}  // namespace stch

#include "Stitch/Scene.ipp"

/**
 * @class stch::Scene
 * @ingroup Scene
 *
 * stch::Scene is the main class you will be interacting with while using this ECS.
 *
 * It is responsible for storing all of your entities and their associated components.
 *
 * Usage example:
 * @code
 * #include <Stitch/Scene.hpp>
 *
 *
 * // Declare a new scene
 *
 * stch::Scene registry;
 *
 *
 * // Handling entities
 *
 * auto entity = registry.emplace();
 * registry.erase(entity);
 *
 * assert(!registry.exists(entity));
 *
 *
 * // Handling components directly
 *
 * entity = registry.emplace();
 *
 * struct Position {
 *     float x, y;
 * };
 *
 * registry.emplace<Position>(entity); // add
 * registry.erase<Position>(entity);   // remove
 *
 * registry.emplace<Position>(entity, Position{1.f, 1.f}); // emplace with parameters
 *
 * auto &position = registry.get<Position>(entity); // retrieve a reference to the component
 * assert(position.y == 1.f);
 *
 *
 * // Iterating over components (systems)
 *
 * struct Velocity {
 *     float x, y;
 * };
 *
 * for (auto entity : registry.view<Position, Velocity>()) {
 *     assert(registry.all_of<Position, Velocity>(entity));
 * }
 *
 * registry.each<Position, Velocity>([](auto id, auto &position, auto &velocity) {
 *     position += velocity;
 * });
 *
 * @endcode
 */
