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

#ifndef ARGS_DISPLAY_ERROR_H
#define ARGS_DISPLAY_ERROR_H

#include <iosfwd>
#include <sstream>
#include <string>


namespace args {
	struct ParseError;

	void report_error(const ParseError& err, std::ostream& o);
}

std::ostream& operator<< (std::ostream& o, const args::ParseError& e);

namespace std {
string to_string(const args::ParseError& e);
}

#endif
