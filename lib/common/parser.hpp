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

#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <iostream>

#include "common/types.hpp"
#include "common/parse_error.hpp"
#include "common/parser_state.hpp"
#include "common/arg.hpp"


namespace args {

template <typename InputIt, typename... ArgParsers>
ParseResultVoid
parse(InputIt argv_begin, InputIt argv_end, ArgParsers&&... arg_parsers) {
	return parse_impl(argv_begin, argv_end, arg_parsers...);
}


template <typename InputIt, typename... ArgParsers>
ParseResultVoid
parse_impl(InputIt argv_begin, InputIt argv_end, ArgParsers&... arg_parsers) {

	OwnedParserState state(argv_begin, argv_end);

	while (state.pos < state.argv.size()) {
		auto res = parse_iter(state, arg_parsers...);

		if (!res.is_ok())
			return res;
	}

	return eval_postcond_iter(state, arg_parsers...);
}


inline ParseResultVoid parse_iter(const ParserState& state) {
	return ParseError(UnknownArg(state));
}

template <typename ArgParser, typename... ArgParsers>
ParseResultVoid parse_iter(OwnedParserState& state,
		ArgParser& arg_parser, ArgParsers&... arg_parsers) {

	auto res = arg_parser.parse(state);

	return is<UnknownArg>(res)? parse_iter(state, arg_parsers...) : res;
}


inline ParseResultVoid eval_postcond_iter(const OwnedParserState& state) {
	return success();
}

template <typename ArgParser, typename... ArgParsers>
ParseResultVoid eval_postcond_iter(const OwnedParserState& state,
		ArgParser& arg_parser, ArgParsers&... arg_parsers) {

	auto res = arg_parser.eval_postcond(state);

	return res.is_ok()? eval_postcond_iter(state, arg_parsers...) : res;
}

}

#endif
