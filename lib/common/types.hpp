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

#ifndef ARGS_TYPES_H
#define ARGS_TYPES_H

#include <limits>
#include <string>
#include <vector>

#include "utils/string_view.hpp"
#include "utils/result.hpp"

#include "common/parse_error.hpp"


namespace args {

#ifndef INT_MAX
constexpr std::size_t INT_MAX = std::numeric_limits<int>::max();
#endif

using uint = unsigned int;

using Str = std::string;

template <bool cond, typename Ret = ParseResultVoid>
using If = typename std::enable_if<cond, Ret>::type;


struct BaseArg {
	uint multiplicity = 0;

	explicit operator bool() const {
		return multiplicity > 0;
	}

	virtual Str to_str() const = 0;
};

}

#endif
