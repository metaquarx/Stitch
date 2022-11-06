// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <cstddef>
#include <functional>

namespace stch::arch {

struct PoolInfo {
	std::size_t m_capacity;
	std::size_t m_size;
};

struct Pool {
	Pool(Pool && other);
	~Pool();

	Pool(const Pool &) = delete;
	Pool & operator=(const Pool &) = delete;
	Pool & operator=(Pool &&) = delete;

	template <typename T>
	static Pool create(PoolInfo & info);

	Pool dupe(PoolInfo & new_info) const;
	std::byte * get(std::size_t row) const;
	void erase(std::size_t row);

private:
	friend struct Container;

	using Destructor = std::function<void(std::byte *)>;
	using UninitialisedMove = std::function<void(std::byte *, std::byte *)>;
	using Swap = std::function<void(std::byte *, std::byte *)>;

	Destructor m_destruct;
	UninitialisedMove m_umove;
	Swap m_swap;

	Pool(
		PoolInfo & info,
		std::size_t type_size,
		Destructor destructor,
		UninitialisedMove umove,
		Swap swap
	);

	PoolInfo * m_storage;
	std::size_t m_type_size;
	std::byte * m_elements;
};

} // namespace stch::arch

#include "Stitch/Pool.ipp"
