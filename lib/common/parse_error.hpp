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

#ifndef ARGS_PARSE_ERROR_H
#define ARGS_PARSE_ERROR_H

#include <string>
#include <type_traits>

#include <boost/mpl/at.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

#include "utils/string_view.hpp"
#include "utils/result.hpp"

#include "common/parser_state.hpp"


namespace args {

struct BaseArg;
struct ParserState;

struct Nothing {};

struct BaseArgError {
	const BaseArg* arg = nullptr;

	BaseArgError() = default;

	explicit BaseArgError(const BaseArg* arg) : arg(arg) {}
};


struct UnknownArg {
	StrView argv_element;
	uint str_off;

	explicit UnknownArg(const ParserState& state)
		: argv_element(state.arg())
		, str_off(state.str_off) {}

	UnknownArg(const UnknownArg&) = default;
	UnknownArg& operator= (const UnknownArg&) = default;
};

struct BoundError : public BaseArgError {
	std::string method_name;

	explicit BoundError(const char* method_name, const BaseArg* arg = nullptr)
		: BaseArgError(arg)
		, method_name(method_name) {}
};

struct InvalidParam : public BaseArgError {
	std::string custom_msg;
	StrView given;

	InvalidParam(std::string custom_msg, StrView given, const BaseArg* arg = nullptr)
		: BaseArgError(arg)
		, custom_msg(std::move(custom_msg))
		, given(given) {}
};

struct InvalidShortoptList : public BaseArgError {
	StrView shortopt_list;

	explicit InvalidShortoptList(
			const StrView& shortopt_list,
			const BaseArg* arg = nullptr)
		: BaseArgError(arg)
		, shortopt_list(std::move(shortopt_list)) {}
};

struct CondFailed : public BaseArgError {
	std::string error_msg;

	CondFailed(std::string error_msg, const BaseArg* arg = nullptr)
		: BaseArgError(arg)
		, error_msg(std::move(error_msg)) {}
};


template <typename... Ts>
struct GenericParseError {
	using Vec = boost::mpl::vector<Ts...>;
	using Var = boost::variant<Ts...>;

	static constexpr std::size_t type_count = sizeof...(Ts);

	Var data;

	GenericParseError(boost::variant<Ts...> data)
		: data(std::move(data)) {}


	template <std::size_t I>
	using at = typename boost::mpl::at_c<Vec, I>::type;

	template <typename T>
	static constexpr std::size_t index_of() {
		using namespace boost::mpl;

		using vec_begin = typename begin<Vec>::type;
		using vec_found = typename find<Vec, T>::type;
		using vec_end   = typename end<Vec>::type;

		static_assert(
			!std::is_same<vec_found, vec_end>::value,
			"Type is not part of ParseError");

		return distance<vec_begin, vec_found>::type::value;
	}

	template <typename T>
	const T& get() const {
		return boost::get<T>(data);
	}

	template <typename T>
	T& get() {
		return boost::get<T>(data);
	}

	void set_arg(const BaseArg* arg) {
		set_arg_iter<0>(arg);
	}

	template <std::size_t I, typename T = void>
	typename std::enable_if<I == sizeof...(Ts)>::type
	set_arg_iter(const BaseArg*) {
	}

	template <std::size_t I>
	typename std::enable_if<I < sizeof...(Ts)>::type
	set_arg_iter(const BaseArg* arg) {
		constexpr bool is_base = std::is_base_of<BaseArgError, at<I>>::value;

		if (data.which() == I && is_base)
			reinterpret_cast<BaseArgError&>(get<at<I>>()).arg = arg;
		else
			set_arg_iter<I+1>(arg);
	}
};

template <typename T, typename... Ts>
static bool is(const GenericParseError<Ts...>& e) {
	return e.data.which() == GenericParseError<Ts...>::template index_of<T>();
}


struct ParseError
	: public GenericParseError<
		Nothing,
		UnknownArg,
		BoundError,
		InvalidParam,
		InvalidShortoptList,
		CondFailed
	> {

	using GenericParseError::GenericParseError;


	bool is_ok() const {
		return is<Nothing>(*this);
	}
};

template <typename T>
using ParseResult = Result<T, ParseError>;

using ParseResultVoid = ParseError;

inline ParseResultVoid success() {
	return ParseError(Nothing());
}

}

#endif
