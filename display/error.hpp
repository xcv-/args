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

#ifndef ARGS_DISPLAY_ERROR_H
#define ARGS_DISPLAY_ERROR_H

#include <iostream>
#include <sstream>
#include <string>

#include "common/parse_error.hpp"


namespace args {

std::string err_arg_str(const BaseArg* arg);


void report_error(const ParseError& err, std::ostream& o = std::cout) {

	static_assert(ParseError::type_count == 6, "Mismatching types in args::report_error");

	switch (err.data.which()) {
		case ParseError::index_of<UnknownArg>(): {
			const auto& e = err.get<UnknownArg>();

			o << "unknown argument: " << e.argv_element << "\n";

		} break;

		case ParseError::index_of<BoundError>(): {
			const auto& e = err.get<BoundError>();

			o << "<internal>: bound check failed in " << e.method_name << ":\n";

			o << "  while parsing " << err_arg_str(e.arg) << "\n";

		} break;

		case ParseError::index_of<InvalidParam>(): {
			const auto& e = err.get<InvalidParam>();

			o << "invalid parameter for " << err_arg_str(e.arg) << ":\n";

			o << "  could not parse '" << e.given << "':\n";
			o << "    " << e.custom_msg << "\n";

		} break;

		case ParseError::index_of<InvalidShortoptList>(): {
			const auto& e = err.get<InvalidShortoptList>();

			o << "shortopt list contains " << err_arg_str(e.arg) << ":\n";

			o << "  list: '" << e.shortopt_list << "':\n";

		} break;

		case ParseError::index_of<CondFailed>(): {
			const auto& e = err.get<CondFailed>();

			o << "condition failed for " << err_arg_str(e.arg) << ":\n";

			o << "  " << e.error_msg << "\n";

		} break;

		case ParseError::index_of<Nothing>():
			o << "expected error but found Nothing\n";
			break;

		default:
			o << "unknown error " << err.data.which() << "\n";
	}
}

}

std::ostream& operator<< (std::ostream& o, const args::ParseError& e) {
	args::report_error(e, o);
	return o;
}

namespace std {

std::string to_string(const args::ParseError& e) {
	std::ostringstream ss;
	::operator<<(ss, e);
	return ss.str();
}

}

#endif
