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

#ifndef ARGS_CONDITION_H
#define ARGS_CONDITION_H

#include <sstream>
#include <tuple>

#include "common/types.hpp"
#include "common/parse_error.hpp"
#include "common/parser_state.hpp"


namespace args {

namespace condition {

ParseResultVoid cond_fail(Str error_msg) {
	return ParseError(CondFailed(std::move(error_msg)));
}


struct True {
	ParseResultVoid operator() (const BaseArg& self, const ParserState& s) const {
		return success();
	}
};

struct False {
	ParseResultVoid operator() (const BaseArg& self, const ParserState& s) const {
		return cond_fail("False");
	}
};


template <typename... Predicates>
struct All {
	std::tuple<Predicates...> predicates;

	static constexpr std::size_t N = sizeof...(Predicates);


	explicit All(Predicates... predicates) : predicates(std::move(predicates)...) {}

	template <typename Self>
	ParseResultVoid operator() (const Self& self, const ParserState& s) const {
		return eval_iter<Self, 0>(self, s);
	}


	template <typename Self, std::size_t I>
	If<I == N> eval_iter(const Self& self, const ParserState& s) const {
		return success();
	}

	template <typename Self, std::size_t I>
	If<I < N> eval_iter(const Self& self, const ParserState& s) const {
		auto res = std::get<I>(predicates)(self, s);

		if (res.is_ok())
			return eval_iter<Self, I+1>(self, s);
		else
			return res;
	}
};


template <typename... Predicates>
struct Any {
	std::tuple<Predicates...> predicates;
	mutable std::vector<ParseError> errors;

	static constexpr std::size_t N = sizeof...(Predicates);


	explicit Any(Predicates... predicates) : predicates(std::move(predicates)...) {}

	template <typename Self>
	ParseResultVoid operator() (const Self& self, const ParserState& s) const {
		return eval_iter<Self, 0>(self, s);
	}


	template <typename Self, std::size_t I>
	If<I == N> eval_iter(const Self& self, const ParserState& s) const {
		std::ostringstream ss("Failed all conditions on " + self->to_str() + ":\n");

		for (const auto& e : errors)
			ss << e;

		errors.clear();
		return cond_fail(ss.str());
	}

	template <typename Self, std::size_t I>
	If<I < N> eval_iter(const Self& self, const ParserState& s) const {
		auto res = std::get<I>(predicates)(self, s);

		if (res.is_ok()) {
			errors.clear();
			return success();
		}
		else {
			errors.emplace_back(std::move(res));
			return eval_iter<Self, I+1>();
		}
	}
};


struct Requires {
	const BaseArg& to_find;

	explicit Requires(const BaseArg& to_find) : to_find(to_find) {}

	ParseResultVoid operator() (const BaseArg& self, const ParserState& s) const {
		using namespace std;
		auto& m = s.matched_args;

		if (find(begin(m), end(m), &to_find) == end(m))
			return success();
		else
			return cond_fail("Requires " + to_find.to_str());
	}
};

struct Conflicts {
	const BaseArg& to_find;

	explicit Conflicts(const BaseArg& to_find) : to_find(to_find) {}

	ParseResultVoid operator() (const BaseArg& self, const ParserState& s) const {
		using namespace std;
		auto& m = s.matched_args;

		if (find(begin(m), end(m), &to_find) != end(m))
			return success();
		else
			return cond_fail("Requires " + to_find.to_str());
	}
};


struct Before {
	const BaseArg& to_find;

	explicit Before(const BaseArg& to_find) : to_find(to_find) {}

	ParseResultVoid operator() (const BaseArg& self, const ParserState& s) const {
		for (const BaseArg* matched : s.matched_args) {
			if (matched == &to_find)
				return cond_fail("Before " + to_find.to_str());

			if (matched == &self)
				break;
		}

		return success();
	}
};

struct After {
	const BaseArg& to_find;

	explicit After(const BaseArg& to_find) : to_find(to_find) {}

	ParseResultVoid operator() (const BaseArg& self, const ParserState& s) const {
		for (const BaseArg* matched : s.matched_args) {
			if (matched == &self)
				return cond_fail("After " + to_find.to_str());

			if (matched == &to_find)
				break;
		}

		return success();
	}
};

struct Min {
	uint min_multiplicity;

	explicit Min(uint min_multiplicity) : min_multiplicity(min_multiplicity) {}

	ParseResultVoid operator() (const BaseArg& self, const ParserState& s) const {
		if (self.multiplicity >= min_multiplicity)
			return success();
		else
			return cond_fail("Min " + std::to_string(min_multiplicity));
	}
};

struct Max {
	uint max_multiplicity;

	explicit Max(uint max_multiplicity) : max_multiplicity(max_multiplicity) {}

	ParseResultVoid operator() (const BaseArg& self, const ParserState& s) const {
		if (self.multiplicity <= max_multiplicity)
			return success();
		else
			return cond_fail("Max " + std::to_string(max_multiplicity));
	}
};



// Constructor wrappers

template <typename... Predicates>
All<Predicates...> all(Predicates... predicates) {
	return All<Predicates...>(std::move(predicates)...);
}

template <typename... Predicates>
Any<Predicates...> any(Predicates... predicates) {
	return Any<Predicates...>(std::move(predicates)...);
}


Requires requires(const BaseArg& to_find) {
	return Requires(to_find);
}

Conflicts conficts(const BaseArg& to_find) {
	return Conflicts(to_find);
}


Before before(const BaseArg& to_find) {
	return Before(to_find);
}

After after(const BaseArg& to_find) {
	return After(to_find);
}


Min min(uint min_multiplicity) {
	return Min(min_multiplicity);
}

Max max(uint max_multiplicity) {
	return Max(max_multiplicity);
}

All<Min,Max> between(uint min_multiplicity, uint max_multiplicity) {
	return all(min(min_multiplicity), max(max_multiplicity));
}

All<Min,Max> exactly(uint multiplicity) {
	return between(multiplicity, multiplicity);
}

}

}


#endif
