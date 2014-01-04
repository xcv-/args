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

#ifndef ARGS_LAMBDA_ARG_H
#define ARGS_LAMBDA_ARG_H

#include <utility>

#include "common/types.hpp"
#include "common/parse_error.hpp"
#include "common/parser_state.hpp"
#include "common/arg.hpp"
#include "functors/condition.hpp"


namespace args {

template <typename T, typename Parser, typename PreCond, typename PostCond>
struct LambdaArg : public Arg<T, PreCond, PostCond, LambdaArg<T, Parser, PreCond, PostCond>> {
	Parser parser;

	LambdaArg(Parser&& parser, PreCond precond, PostCond postcond)
		: Arg<T, PreCond, PostCond, LambdaArg>(
				std::move(precond),
				std::move(postcond))

		, parser(std::move(parser)) {}

	ParseResult<T> parse_impl(ParserState& s) {
		return parser(*this, s);
	}

	virtual Str to_str() const override {
		return "(lambda)";
	}
};


template <
	typename T,
	typename Parser,
	typename PreCond = condition::True,
	typename PostCond = condition::True
>
LambdaArg<T, Parser, PreCond, PostCond>
lambda_arg(
		Parser parser,
		PreCond precond = PreCond(),
		PostCond postcond = PostCond()) {

	return LambdaArg<T, Parser, PreCond, PostCond> {
		std::move(parser),
		std::move(precond),
		std::move(postcond)
	};
}

}

#endif
