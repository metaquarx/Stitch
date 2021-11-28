// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Stitch/Entity.hpp"

#include <functional>
#include <queue>
#include <vector>

namespace stch {

/**
 * @brief      Component pool for a single type.
 */
class ComponentPool {
public:
	//////////////////// Meta ////////////////////
	/**
	 * @brief      Constructs a new instance.
	 *
	 * @param[in]  type_size   Sizeof the type to be held
	 * @param[in]  destructor  Function that calls the type's destructor
	 * @param[in]  move        Function that moves lhs to an uninitialised location at rhs
	 */
	ComponentPool(std::size_t type_size,
	  std::function<void(const std::byte *)> destructor,
	  std::function<void(std::byte *, std::byte *)> move);

	/**
	 * @brief      Move constructor
	 *
	 * @param      other  Object to move from
	 */
	ComponentPool(ComponentPool &&other);
	/**
	 * @brief      Move assignment
	 *
	 * @param      other  Object to move from
	 *
	 * @return     The result of the assignment
	 */
	ComponentPool &operator=(ComponentPool &&other);
	/**
	 * @brief      Destructor
	 */
	~ComponentPool();

	//////////////////// Capacity ////////////////////

	/**
	 * @brief      Gets the size of the internal used space.
	 *
	 * @return     The size.
	 */
	std::size_t size() const;

	/**
	 * @brief      Gets the size of the internal allocated space.
	 *
	 * @return     Allocated space.
	 */
	std::size_t capacity() const;

	/**
	 * @brief      Increases the internal size of the component pool.
	 *
	 * @param[in]  size  New size
	 *
	 * If size is greater than the current `capacity()`, then new storage is allocated.
	 * Otherwise nothing happens.
	 *
	 * If your goal is to shrink usage, call `repack()`.
	 */
	void reserve(std::size_t size);

	//////////////////// Element Access ////////////////////

	/**
	 * @brief      Get (void) pointer to object at the specified index. This should then
	 * be casted to the right type.
	 *
	 * @param[in]  index  The index
	 *
	 * @return     Pointer to the object requested.
	 */
	const std::byte *operator[](std::size_t index) const;

	/**
	 * @brief      Same as above except non-const
	 *
	 * @param[in]  index  The index
	 *
	 * @return     Pointer to the object requested.
	 */
	std::byte *operator[](std::size_t index);

	//////////////////// Modifiers ////////////////////

	/**
	 * @brief      Clears all components stored within.
	 */
	void clear();

	/**
	 * @brief      Requests removal of unused capacity
	 *
	 * After this operation, `size()` will match `capacity()`
	 */
	void repack();

	//////////////////// End ////////////////////

	/*
	 * @brief      Get destructor of the stored type
	 *
	 * @return     Pointer to destructor function.
	 */
	std::function<void(const std::byte *)> get_dtor();

	// EntityID -> internal
	// num_max = not in use
	std::vector<std::size_t> sparse;

	// internal -> EntityID
	// num_max = reclaimable
	// size is (capacity)
	std::vector<EntityID> packed;

	// index -> internal
	// packed.size() - size is (size)
	std::priority_queue<std::size_t, std::vector<std::size_t>, std::greater<std::size_t>> reclaimable_packed;

private:
	// internal * element_size -> T
	// allocated size is the same as packed.size()
	std::byte *data;

	std::function<void(const std::byte *)> dtor;
	std::function<void(std::byte *, std::byte *)> umove;
	std::size_t element_size;

	// non copyable
	ComponentPool(const ComponentPool &other);
	ComponentPool &operator=(const ComponentPool &other);
};

}  // namespace stch
