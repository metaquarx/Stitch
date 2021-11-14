// SPDX-FileCopyrightText: 2021 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Stitch/Entity.hpp"

#include <typeindex>
#include <vector>

namespace stch {

class Scene;

/**
 * @brief      An iterable view into a scene
 */
class View {
public:
	View(Scene &scene, const std::vector<std::type_index> &requested = {});

	/**
	 * @brief      An iterator into a view
	 */
	class Iterator {
	public:
		Iterator(View &parent, unsigned index);

		/**
		 * @brief      Dereference operator
		 */
		EntityID operator*();

		/**
		 * @brief      Equality operator.
		 */
		bool operator==(const Iterator &rhs) const;
		/**
		 * @brief      Inequality operator.
		 */
		bool operator!=(const Iterator &rhs) const;

		/**
		 * @brief      Increment operator. (pre)
		 */
		Iterator &operator++();
		/**
		 * @brief      Increment operator. (post)
		 */
		Iterator operator++(int);

	private:
		View &parent;
		unsigned index;	 // indices are tracked from pools[requested.front()].packed
	};

	/**
	 * @brief      Get begin iterator
	 */
	Iterator begin();
	/**
	 * @brief      Get end iterator
	 */
	Iterator end();

private:
	Scene &scene;
	std::vector<std::type_index> requested;
	bool request_invalid;

	bool is_valid(unsigned index);
};

}  // namespace stch
