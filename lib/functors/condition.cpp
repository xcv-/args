#include "functors/condition.hpp"

using namespace args;
using namespace args::condition;


ParseResultVoid Requires::operator() (const BaseArg& self, const ParserState& s) const {
	using namespace std;
	auto& m = s.matched_args;

	if (find(begin(m), end(m), &to_find) == end(m))
		return success();
	else
		return cond_fail("Requires " + to_find.to_str());
}

ParseResultVoid Conflicts::operator() (const BaseArg& self, const ParserState& s) const {
	using namespace std;
	auto& m = s.matched_args;

	if (find(begin(m), end(m), &to_find) != end(m))
		return success();
	else
		return cond_fail("Conflicts " + to_find.to_str());
}


ParseResultVoid Before::operator() (const BaseArg& self, const ParserState& s) const {
	for (const BaseArg* matched : s.matched_args) {
		if (matched == &to_find)
			return cond_fail("Before " + to_find.to_str());

		if (matched == &self)
			break;
	}

	return success();
}

ParseResultVoid After::operator() (const BaseArg& self, const ParserState& s) const {
	for (const BaseArg* matched : s.matched_args) {
		if (matched == &self)
			return cond_fail("After " + to_find.to_str());

		if (matched == &to_find)
			break;
	}

	return success();
}


ParseResultVoid Min::operator() (const BaseArg& self, const ParserState& s) const {
	if (self.multiplicity >= min_multiplicity)
		return success();
	else
		return cond_fail("Min " + std::to_string(min_multiplicity));
}

ParseResultVoid Max::operator() (const BaseArg& self, const ParserState& s) const {
	if (self.multiplicity <= max_multiplicity)
		return success();
	else
		return cond_fail("Max " + std::to_string(max_multiplicity));
}
