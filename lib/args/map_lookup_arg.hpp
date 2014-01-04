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

#ifndef ARGS_MAP_LOOKUP_ARG_H
#define ARGS_MAP_LOOKUP_ARG_H

#include <unordered_map>
#include <utility>

#include "common/types.hpp"
#include "common/parse_error.hpp"
#include "common/parser_state.hpp"
#include "common/arg.hpp"
#include "functors/condition.hpp"


namespace args {

template <typename T, typename PreCond, typename PostCond>
struct MapLookupArg : public Arg<T, PreCond, PostCond, MapLookupArg<T, PreCond, PostCond>> {
	std::unordered_map<Str, T> lookup_map;

	MapLookupArg(std::unordered_map<Str, T> lookup_map,
			PreCond precond, PostCond postcond)
		: Arg<T, PreCond, PostCond, MapLookupArg>(
				std::move(precond),
				std::move(postcond))
		, lookup_map(std::move(lookup_map)) {}


	ParseResult<T> parse_impl(ParserState& s) {
		using R = ParseResult<T>;

		if (!s.bounds_check())
			return R::err(ParseError(BoundError("MapLookupArg::parse()")));

		const auto& arg = s.arg_next();

		if (s.str_off != 0)
			return R::err(ParseError(InvalidShortoptList(arg)));

		auto i = lookup_map.find(arg.str());

		if (i == end(lookup_map)) {
			return R::err(ParseError(InvalidParam(
				"expected one of " + fmt_keys("(", ", ", ")"),
				arg)));
		} else {
			return R::ok(i->second);
		}
	}

	virtual Str to_str() const override {
		return fmt_keys("", ", ", "");
	}


	Str fmt_keys(StrView lhs, StrView sep, StrView rhs) const {
		auto i   = std::begin(lookup_map);
		auto end = std::end(lookup_map);

		Str s = lhs.c_str();

		if (!lookup_map.empty()) {
			s += i->first;

			while (++i != end) {
				s += sep.c_str();
				s += i->first;
			}
		}

		s += rhs.c_str();

		return s;
	}
};



template <
	typename T,
	typename PreCond = condition::True,
	typename PostCond = condition::True
>
MapLookupArg<T, PreCond, PostCond>
map_lookup_arg(
		std::unordered_map<Str, T> lookup_map,
		PreCond precond = condition::True(),
		PostCond postcond = condition::True()) {

	return MapLookupArg<T, PreCond, PostCond> {
		std::move(lookup_map),
		std::move(precond),
		std::move(postcond)
	};
}

}

#endif
