// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Stitch/Pool.hpp"

namespace stch::arch {

template <typename T>
Pool Pool::create(PoolInfo & info) {
	return Pool(
		info,
		sizeof(T),
		[](std::byte * p) {
			reinterpret_cast<T *>(p)->~T();
		},
		[](std::byte * s, std::byte * d) {
			new (d) T(std::move(*reinterpret_cast<T *>(s)));
		},
		[](std::byte * a, std::byte * b) {
			T temp(std::move(*reinterpret_cast<T *>(a)));
			reinterpret_cast<T *>(a)->~T();
			new (a) T(std::move(*reinterpret_cast<T *>(b)));
			reinterpret_cast<T *>(b)->~T();
			new (b) T(std::move(temp));
		}
	);
}

} // namespace stch::arch
