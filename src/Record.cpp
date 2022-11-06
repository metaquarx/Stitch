// SPDX-FileCopyrightText: 2022 metaquarx <metaquarx@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "Stitch/Record.hpp"

namespace stch::arch {

Record::Record(
	Container & location,
	std::size_t row)
: m_location(&location)
, m_row(row) {
}

Record::Record(Record && other)
: m_location(other.m_location), m_row(other.m_row) {
}

Record & Record::operator=(Record && other) {
	m_location = other.m_location;
	m_row = other.m_row;
	return *this;
}

}
