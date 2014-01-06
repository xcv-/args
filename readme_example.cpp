#include "args_all.hpp"

int main(int argc, char** argv) {
	using namespace args::condition;

	auto flag = args::flag('f', "flag")
		.description("a flag"); // description to show in the help message

	auto integer_flag = args::flag<int>("integer")
		.metavars("INT") // parameter name to display in the help message
		.description("takes an integer");

	auto multi_param_flag = args::flag<int,bool,int>('m', "multi-param")
		.metavars("INT BOOL INT2")
		.description("this is used to show how multiple parameters can be passed");

	auto requires_flag = args::flag('r', "requires-flag", True(), requires(flag))
		.description("does nothing, but requires -f or --flag");

	auto help_flag = args::flag('h', "help", True(), max(0))
		.description("print this message and exit");

	auto once_flag = args::flag('o', "once", True(), min(1))
		.description("can only be specified once");

	enum class Command { BUILD, RUN };
	auto command = args::map_lookup_arg<Command>({
		{ "build", Command::BUILD },
		{ "run",   Command::RUN   }
	}, max(0), min(1))
	.description("available commands");

	enum class Target { ALL, CURRENT };
	auto target = args::map_lookup_arg<Target>({
		{ "all",     Target::ALL     },
		{ "current", Target::CURRENT }
	}, all(after(command), max(0)), min(1))
	.description("available targets");


	// flags require the flags() parser because they need extra logic for shortopts
	// and the endopts mark. anything else can be in a args() parser.
	// order in which they are specified is the order in which the parser will
	// try to parse them.
	auto flag_parser = args::flags(flag, integer_flag, multi_param_flag,
	                               requires_flag, help_flag, once_flag);

	auto argument_parser = args::args(command, target);

	// takes two InputIterators (args_begin and args_end). value_type must be
	// convertible to args::StrView (const char* or std::string)
	auto result = args::parse(argv+1, argv+argc, flag_parser, argument_parser);

	using std::cout;
	using std::boolalpha;

	if (result.is_ok()) {
		cout << "Flag has been given " << flag.multiplicity << " times\n";

		cout << "Found --integer values: ";

		// .result is a vector<T>. In the case of flags, T is a tuple
		// of the parameter types, in this case, tuple<int>
		for (const auto& tup : integer_flag.result)
			cout << std::get<0>(tup) << ' ';
		cout << "\n";

		cout << "Found --multi-param values: ";

		for (const auto& tup : multi_param_flag.result)
			cout << '(' << boolalpha << std::get<0>(tup) << ','
			     << ' ' << boolalpha << std::get<1>(tup) << ','
			     << ' ' << boolalpha << std::get<2>(tup) << ") ";

		cout << "\n";

		cout << "Found -r or --requires-flag? " << boolalpha <<
			(bool) requires_flag << '\n';

		cout << "Command is BUILD? " << boolalpha <<
			(command.result.front() == Command::BUILD) << '\n';

		cout << "Target is ALL? " << boolalpha <<
			(target.result.front() == Target::ALL) << '\n';

	} else if (help_flag) {
		cout << "Help flag found\n";

		cout << "Usage: "
		     << argv[0] << " <flags> COMMAND <flags> TARGET <flags>"
		     << "\n\n";

		cout << "Fixed arguments:\n";
		// indent with two spaces and put the description 4 spaces away
		args::print_help(argument_parser, {"  ", 4}, cout);
		cout << "\n";

		cout << "Flags:\n";
		args::print_help(flag_parser, {"  ", 4}, cout);

	} else {
		cout << "Error ocurred:\n";

		args::report_error(result, cout);
	}
}
