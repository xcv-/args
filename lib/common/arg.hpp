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

#ifndef ARGS_ARG_H
#define ARGS_ARG_H

#include "common/types.hpp"
#include "common/parser_state.hpp"
#include "functors/condition.hpp"


namespace args {

template <typename T>
struct ArgState : public BaseArg {
	std::vector<T> result;
};

template <typename T, typename PreCond, typename PostCond, typename Derived>
struct Arg : public ArgState<T> {
	PreCond precond;
	PostCond postcond;
	StrView doc_description;

	Arg(PreCond precond, PostCond postcond)
		: ArgState<T>()
		, precond(std::move(precond))
		, postcond(std::move(postcond)) {}

	ParseResultVoid parse(ParserState& s) {
		auto res = success();

		derived().parse_impl(s)

		.if_ok([&](T&& parsed) {
			this->result.emplace_back(std::move(parsed));
		})

		.if_err([&](ParseError&& err) {
			res = err;
		});

		return res;
	}

	Derived& description(StrView desc) & {
		this->doc_description = desc;
		return derived();
	}

	Derived&& description(StrView desc) && {
		this->doc_description = desc;
		return std::move(derived());
	}

private:
	Derived& derived() & {
		return *static_cast<Derived*>(this);
	}

	Derived&& derived() && {
		return std::move(*static_cast<Derived*>(this));
	}
};


template <typename... ArgTs>
struct ArgParser {
	static constexpr std::size_t N = sizeof...(ArgTs);

private:

	std::tuple<ArgTs&...> args;


	template <std::size_t I>
	If<I == N> parse_iter(OwnedParserState& state) {
		return ParseError(UnknownArg(state));
	}

	template <std::size_t I>
	If<I < N> parse_iter(OwnedParserState& state) {
		auto state_clone = ParserState(state);

		auto& arg = std::get<I>(args);

		const auto& c_arg = arg;
		auto res = arg.precond(c_arg, state_clone);

		if (!res.is_ok())
		  return is<CondFailed>(res) ? parse_iter<I+1>(state) : res;

		res = arg.parse(state_clone);

		if (res.is_ok()) {
			arg.multiplicity++;
			state.owned_matched_args.push_back(&arg);
			state.update(state_clone);
			return res;

		} else if (is<UnknownArg>(res)) {
			return parse_iter<I+1>(state);

		} else {
			res.set_arg(&arg);
			return res;
		}
	}


	template <std::size_t I>
	If<I == N> eval_postcond_iter(const ParserState& state) const {
		return success();
	}

	template <std::size_t I>
	If<I < N> eval_postcond_iter(const ParserState& s) const {
		const auto& arg = std::get<I>(args);
		auto res = arg.postcond(arg, s);

		if (res.is_ok()) {
			return eval_postcond_iter<I+1>(s);
		} else {
			res.set_arg(&arg);
			return res;
		}
	}

public:
	const decltype(args)& const_args = args;


	ArgParser(ArgTs&... args)
		: args(std::tie(args...)) {}

	ParseResultVoid parse(OwnedParserState& state) {
		return parse_iter<0>(state);
	}

	ParseResultVoid eval_postcond(const ParserState& state) const {
		return eval_postcond_iter<0>(state);
	}
};


template <typename... ArgTs>
ArgParser<ArgTs...> args(ArgTs&... args) {
	return ArgParser<ArgTs...>(args...);
}

}


#endif
