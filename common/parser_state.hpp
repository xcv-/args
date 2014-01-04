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

#ifndef ARGS_PARSER_STATE_H
#define ARGS_PARSER_STATE_H

#include "utils/string_view.hpp"


namespace args {

struct BaseArg;


template <typename Container>
bool bounds_check(int pos, const Container& cont) {
	return pos < static_cast<int>(cont.size());
}


struct ParserState {
	uint pos = 0;
	uint str_off = 0;

	const std::vector<const BaseArg*>& matched_args;
	const std::vector<StrView>& argv;

	ParserState(
			const std::vector<const BaseArg*>& matched_args,
			const std::vector<StrView>& argv)
		: matched_args(matched_args)
		, argv(argv) {}

	ParserState(const ParserState&) = default;


	bool bounds_check() const {
		return ::args::bounds_check(pos, argv);
	}

	bool offset_check() const {
		return ::args::bounds_check(str_off, arg());
	}

	const StrView& arg() const {
		return argv[pos];
	}

	const StrView& arg_next() {
		auto& a = arg();
		pos++;
		return a;
	}

	char ch() const {
		return arg()[str_off];
	}

	void update(ParserState& clone) {
		pos = clone.pos;
		str_off = clone.str_off;
	}
};


struct OwnedParserState : public ParserState {
	std::vector<const BaseArg*> owned_matched_args = {};

	template <typename InputIt>
	OwnedParserState(InputIt argv_begin, InputIt argv_end)
		: ParserState(owned_matched_args, owned_argv)
		, owned_argv(argv_begin, argv_end) {

		owned_matched_args.reserve(argv.size());
	}

private:
	const std::vector<StrView> owned_argv;
};


}

#endif
