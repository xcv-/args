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

#ifndef ARGS_PARAM_PARSERS_H
#define ARGS_PARAM_PARSERS_H

#include <sstream>

#include "common/types.hpp"
#include "common/parse_error.hpp"
#include "common/parser_state.hpp"

#include "args/flag/param_parser.hpp"


namespace args {

template <>
struct ParamParser<bool> {
	using Result = ParseResult<bool>;

	static Result parse(ParserState& s) {
		if (!s.bounds_check())
			return Result::err(BoundError("ParamParser<bool>"));

		const auto& arg = s.arg_next();

		if (s.str_off != 0)
			return Result::err(ParseError(InvalidShortoptList(arg)));
		else if (arg == "true")
			return Result::ok(true);
		else if (arg == "false")
			return Result::ok(false);
		else
			return Result::err(ParseError(InvalidParam("expected 'true' or 'false'", arg)));
	}
};

template <>
struct ParamParser<int> {
	using Result = ParseResult<int>;

	static ParseResult<int> parse(ParserState& s) {
		if (!s.bounds_check())
			return Result::err(BoundError("ParamParser<int>"));

		const auto& arg = s.arg_next();

		int result;

		if (s.str_off != 0)
			return Result::err(ParseError(InvalidShortoptList(arg)));
		else if (std::istringstream(arg.c_str()) >> result)
			return Result::ok(result);
		else
			return Result::err(ParseError(InvalidParam("could not parse int", arg)));
	}
};

}

#endif
