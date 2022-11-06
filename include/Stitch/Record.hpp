// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Stitch/Container.hpp"

namespace stch::arch {

struct Record {
	Record(Record && other);
	Record & operator=(Record && other);
	Record(Container & location, std::size_t row);
	~Record() = default;

	Record() = delete;
	Record & operator=(const Record &) = delete;

	Container * m_location;
	std::size_t m_row;
};

} // namespace stch::arch
