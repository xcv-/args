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

#ifndef ARGS_FLAG_H
#define ARGS_FLAG_H

#include <algorithm>
#include <iostream>
#include <tuple>

#include "common/types.hpp"
#include "common/parse_error.hpp"
#include "common/parser_state.hpp"
#include "common/arg.hpp"
#include "functors/condition.hpp"

#include "flag/param_parser.hpp"


namespace args {

struct FlagMatcher {
	static constexpr char no_shortopt = '\0';

	char shortopt;
	Str longopt;

	FlagMatcher(char shortopt, Str longopt)
		: shortopt(shortopt)
		, longopt(std::move(longopt)) {}


	bool match_shortopt(const ParserState& s) const {
		return shortopt != no_shortopt && s.ch() == shortopt;
	}

	bool match_longopt(const ParserState& s) const {
		return s.str_off == 0 && s.arg() == longopt;
	}

	bool match(ParserState& s) const {
		if (match_shortopt(s)) {
			advance_shortopt(s);
			return true;
		}

		else if (match_longopt(s)) {
			s.pos++;
			return true;
		}

		else
			return false;
	}


	static void advance_shortopt(ParserState& s) {
		s.str_off++;

		if (s.str_off == s.arg().size()) {
			s.str_off = 0;
			s.pos++;
		}
	}
};


template <typename PreCond, typename PostCond, typename... Params>
class Flag : public Arg<
				std::tuple<Params...>, PreCond, PostCond,
				Flag<PreCond, PostCond, Params...>
			> {

	using ParamsTuple = std::tuple<Params...>;
	using TupResult = ParseResult<ParamsTuple>;

	static constexpr std::size_t N = sizeof...(Params);

	FlagMatcher matcher;


	template <std::size_t I>
	static If<I == N>
	parse_params_iter(ParserState&, ParamsTuple&) {
		return success();
	}

	template <std::size_t I>
	static If<I < N>
	parse_params_iter(ParserState& s, ParamsTuple& tup) {
		using Type = typename std::tuple_element<I, ParamsTuple>::type;

		auto res = success();

		ParamParser<Type>::parse(s)

		.if_ok([&](Type&& value) {
			res = parse_params_iter<I+1>(s, tup);

			if (res.is_ok())
				std::get<I>(tup) = std::move(value);
		})

		.if_err([&](ParseError&& err) {
			res = std::move(err);
		});

		return res;
	}

	TupResult parse_params(ParserState& s) {
		ParamsTuple tup;

		auto res = parse_params_iter<0>(s, tup);

		if (res.is_ok())
			return TupResult::ok(std::move(tup));
		else
			return TupResult::err(std::move(res));
	}

	void fmt_metavars(Str& s) const {
		if (!doc_metavars.empty()) {
			s += " ";
			s += doc_metavars;
		}
	}

public:

	Str doc_metavars;


	Flag(char shortopt, Str longopt, PreCond precond, PostCond postcond)
		: Arg<ParamsTuple, PreCond, PostCond, Flag>(
				std::move(precond), std::move(postcond))
		, matcher(shortopt, std::move(longopt)) {}

	Flag(Flag&&) = default;


	TupResult parse_impl(ParserState& s) {
		if (!s.bounds_check() || !s.offset_check())
			return TupResult::err(BoundError("Flag::parse"));

		if (matcher.match(s))
			return parse_params(s);
		else
			return TupResult::err(ParseError(UnknownArg(s)));
	}


	Flag& metavars(Str metavars) & {
		this->doc_metavars = std::move(metavars);
		return *this;
	}

	Flag&& metavars(Str metavars) && {
		this->doc_metavars = std::move(metavars);
		return std::move(*this);
	}


	virtual std::string to_str() const override {
		Str result;

		if (matcher.shortopt != FlagMatcher::no_shortopt) {
			result += "-";
			result += matcher.shortopt;
			fmt_metavars(result);
			result += ", ";
		}

		result += matcher.longopt;
		fmt_metavars(result);

		return result;
	}
};


template <typename... FlagTs>
class FlagParser : public ArgParser<FlagTs...> {

	bool found_endflags = false;

public:

	using ArgParser<FlagTs...>::ArgParser;

	ParseResultVoid parse(OwnedParserState& s) {
		if (s.bounds_check() && s.arg() == "--")
			found_endflags = true;

		const auto& arg = s.arg();

		if (found_endflags || arg == "-" || (arg.size() >= 1 && arg[0] != '-'))
			return ParseError(UnknownArg(s));

		if (s.str_off == 0 && arg.size() >= 2 && arg[0] == '-' && arg[1] != '-')
			FlagMatcher::advance_shortopt(s);

		return this->ArgParser<FlagTs...>::parse(s);
	}
};


// public API

template <
	typename... Ts,
	typename PreCond = condition::True,
	typename PostCond = condition::True
>
Flag<PreCond, PostCond, Ts...>
flag(char shortopt, Str longopt,
		PreCond precond = PreCond(),
		PostCond postcond = PostCond()) {

	return {
		shortopt,
		std::move(longopt),
		std::move(precond),
		std::move(postcond)
	};
}

template <
	typename... Ts,
	typename PreCond = condition::True,
	typename PostCond = condition::True
>
Flag<PreCond, PostCond, Ts...>
flag(Str longopt, PreCond precond = PreCond(), PostCond postcond = PostCond()) {
	return flag<Ts...>(
			FlagMatcher::no_shortopt,
			std::move(longopt),
			std::move(precond),
			std::move(postcond));
}

template <typename... FlagTs>
FlagParser<FlagTs...> flags(FlagTs&... flags) {
	return FlagParser<FlagTs...>(flags...);
}

}

#endif
