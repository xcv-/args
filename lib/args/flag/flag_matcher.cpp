#include "args/flag/flag_matcher.hpp"

using namespace args;


FlagMatcher::FlagMatcher(char shortopt, Str longopt)
	: shortopt(shortopt)
	, longopt(std::move(longopt)) {}


bool FlagMatcher::match_shortopt(const ParserState& s) const {
	return shortopt != no_shortopt && s.ch() == shortopt;
}

bool FlagMatcher::match_longopt(const ParserState& s) const {
	return s.str_off == 0 && s.arg() == longopt;
}


bool FlagMatcher::match(ParserState& s) const {
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


void FlagMatcher::advance_shortopt(ParserState& s) {
	s.str_off++;

	if (s.str_off == s.arg().size()) {
		s.str_off = 0;
		s.pos++;
	}
}
