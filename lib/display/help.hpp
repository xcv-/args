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

#ifndef ARGS_DISPLAY_HELP_H
#define ARGS_DISPLAY_HELP_H

#include <array>
#include <iostream>

#include "utils/string_view.hpp"

#include "common/types.hpp"
#include "common/arg.hpp"


namespace args {

struct HelpFmt {
	StrView indent = "";
	std::size_t padding = 1;

	HelpFmt() = default;
	HelpFmt(const HelpFmt&) = default;
	HelpFmt& operator=(const HelpFmt&) = default;

	HelpFmt(StrView indent, std::size_t padding)
		: indent(indent)
		, padding(padding) {}
};


template <std::size_t I, std::size_t N, typename... ArgTs>
typename std::enable_if<I == N>::type
print_help_strs_iter(const ArgParser<ArgTs...>& arg_parser,
		std::array<Str, N>& lhss, std::array<StrView, N>& rhss,
		std::size_t& max_lhs_len) {
}

template <std::size_t I, std::size_t N, typename... ArgTs>
typename std::enable_if<I < N>::type
print_help_strs_iter(
		const ArgParser<ArgTs...>& arg_parser,
		std::array<Str, N>& lhss, std::array<StrView, N>& rhss,
		std::size_t& max_lhs_len) {

	const auto& arg = std::get<I>(arg_parser.const_args);

	auto& lhs  = std::get<I>(lhss);
	auto& rhs  = std::get<I>(rhss);

	lhs = arg.to_str();
	rhs = arg.doc_description;

	max_lhs_len  = std::max(max_lhs_len, lhs.size());

	print_help_strs_iter<I+1, N, ArgTs...>(arg_parser, lhss, rhss, max_lhs_len);
}


template <typename... ArgTs>
void print_help(const ArgParser<ArgTs...>& arg_parser,
		HelpFmt fmt = HelpFmt(), std::ostream& o = std::cout) {

	constexpr auto N = ArgParser<ArgTs...>::N;

	std::array<Str, N> lhss;
	std::array<StrView, N> rhss;
	std::size_t max_lhs_len = 0;

	print_help_strs_iter<0, N, ArgTs...>(arg_parser, lhss, rhss, max_lhs_len);

	auto flags = o.flags();
	auto fill = o.fill();
	auto width = o.width();
	o.fill(' ');
	o.setf(std::ios_base::left);

	for (std::size_t i = 0; i < N; i++) {
		o << fmt.indent;
		o.width(max_lhs_len + fmt.padding);
		o << lhss[i];
		o << rhss[i];
		o << '\n';
	}

	o.width(width);
	o.fill(fill);
	o.flags(flags);
}

}

#endif
