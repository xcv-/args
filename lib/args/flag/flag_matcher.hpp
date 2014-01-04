#ifndef ARGS_FLAG_MATCHER_H
#define ARGS_FLAG_MATCHER_H

#include <utility>

#include "common/types.hpp"
#include "common/parser_state.hpp"


namespace args {

struct FlagMatcher {
	static constexpr char no_shortopt = '\0';

	char shortopt;
	Str longopt;

	FlagMatcher(char shortopt, Str longopt);

	bool match_shortopt(const ParserState& s) const;

	bool match_longopt(const ParserState& s) const;

	bool match(ParserState& s) const;

	static void advance_shortopt(ParserState& s);
};

}

#endif
