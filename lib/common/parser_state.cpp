#include "parser_state.hpp"

using namespace args;


ParserState::ParserState(
		const std::vector<const BaseArg*>& matched_args,
		const std::vector<StrView>& argv)
	: matched_args(matched_args)
	, argv(argv) {}


bool ParserState::bounds_check() const {
	return ::args::bounds_check(pos, argv);
}

bool ParserState::offset_check() const {
	return ::args::bounds_check(str_off, arg());
}

const StrView& ParserState::arg() const {
	return argv[pos];
}

const StrView& ParserState::arg_next() {
	auto& a = arg();
	pos++;
	return a;
}

char ParserState::ch() const {
	return arg()[str_off];
}

void ParserState::update(ParserState& clone) {
	pos = clone.pos;
	str_off = clone.str_off;
}

