#include "error.hpp"

#include "common/types.hpp"
#include "common/parse_error.hpp"


std::string err_arg_str(const args::BaseArg* arg) {
	return arg
		? arg->to_str() + " (" + std::to_string(arg->multiplicity) + ")"
		: "(null)";
}


void args::report_error(const args::ParseError& err, std::ostream& o) {
	using namespace args;

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


std::ostream& operator<< (std::ostream& o, const args::ParseError& e) {
	args::report_error(e, o);
	return o;
}


std::string std::to_string(const args::ParseError& e) {
	std::ostringstream s;
	args::report_error(e, s);
	return s.str();
}
