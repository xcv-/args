/*
 * Copyright 2014 xcv_
 *
 * This file is part of Args.
 *
 *  Args is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Args is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Args.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef ARGS_STRING_VIEW_H
#define ARGS_STRING_VIEW_H

#include <cstring>
#include <iostream>
#include <string>


namespace args {

class StrView {
	explicit StrView(const char* c_str, std::size_t size)
		: m_c_str(c_str), m_size(size) {}

	const char* m_c_str;
	std::size_t m_size;

public:
	StrView() : StrView("", 0) {}
	StrView(const StrView&) = default;
	StrView& operator= (const StrView&) = default;

	StrView(const char* c_str) : StrView(c_str, std::strlen(c_str)) {}

	StrView(const std::string& str) : StrView(str.c_str(), str.size()) {}


	const char* c_str() const {
		return m_c_str;
	}

	std::string str() const {
		return c_str();
	}

	std::size_t size() const {
		return m_size;
	}

	char operator[](std::size_t pos) const {
		return m_c_str[pos];
	}
};

}

bool operator== (const args::StrView& v1, const args::StrView& v2) {
	return v1.size() == v2.size() && strncmp(v1.c_str(), v2.c_str(), v1.size()) == 0;
}

std::ostream& operator<< (std::ostream& o, const args::StrView& v) {
	return o << v.c_str();
}


#endif
