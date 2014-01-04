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

#include "args_all.hpp"

using std::cout;
using std::enable_if;
using std::get;
using std::size_t;
using std::string;
using std::tuple;


template <size_t I, typename... Ts>
typename enable_if<I == sizeof...(Ts)>::type
display(const tuple<Ts...>& tup) {
}

template <size_t I, typename... Ts>
typename enable_if<I < sizeof...(Ts)>::type
display(const tuple<Ts...>& tup) {
	cout << get<I>(tup) << " ";
	display<I+1, Ts...>(tup);
}


template <typename PreC, typename PostC, typename... Params>
void flag_summary(const string& name, const args::Flag<PreC, PostC, Params...>& arg) {
	cout << name << " summary:\n";
	cout << "  multiplicity: " << arg.multiplicity << "\n";

	if (sizeof...(Params) > 0) {
		for (const auto& params : arg.result) {
			cout << "  > ";
			display<0, Params...>(params);
			cout << "\n";
		}
	}
}

#define FLAG_SUMMARY(f) flag_summary("\n" #f, f)


int main(int argc, char** argv) {
	using namespace args::condition; // precondition/postconditioin functors

	// -h
	// --help
	auto help_flag = args::flag('h', "--help", True(), max(0))
		.description("print this message and exit");

	// no minimum or maximum multiplicity
	// -f
	// --flag
	auto plain_flag = args::flag('f', "--flag")
		.description("this doesn't do anything at all, just a fancy flag");

	// -b <bool>
	// --bool <bool>
	auto bool_flag = args::flag<bool>('b', "--bool")
		.metavars("BOOL")
		.description("pass a boolean");

	// -i <int>
	// --int <int>
	auto int_flag = args::flag<int>('i', "--int", True(), min(1))
		.metavars("INT")
		.description("pass an integer");

	// (must be after -b or --bool)
	// -m <bool> <int>
	// --multi <bool> <int>
	auto multi_flag = args::flag<bool, int>('m', "--multi",
			True(),
			all(exactly(1), after(bool_flag)))
		.metavars("BOOL INT")
		.description("pass both a boolean and an integer");


	// build or run
	enum class TestCmd {
		BUILD=0, RUN=1
	};

	auto testcmd_arg = args::map_lookup_arg<TestCmd>({
				{ "build", TestCmd::BUILD },
				{ "run",   TestCmd::RUN   }}, max(0), min(1))
		.description("command to execute");

	// all or none
	enum class TestCmdInner {
		ALL=0, NONE=1
	};

	auto testcmdinner_arg = args::map_lookup_arg<TestCmdInner>({
				{ "all",  TestCmdInner::ALL  },
				{ "none", TestCmdInner::NONE }}, after(testcmd_arg), exactly(1))
		.description("subject to execute the command on");

	// example: ./a.out -b false build --int 314 -fm true 42 all -fb true

	auto flag_parser = args::flags(help_flag, plain_flag, bool_flag, int_flag, multi_flag);
	auto arg_parser = args::args(testcmd_arg, testcmdinner_arg);

	auto result = args::parse(argv+1, argv+argc, flag_parser, arg_parser);

	if (result.is_ok()) {
		cout << "success!\n" << std::boolalpha;

		FLAG_SUMMARY(plain_flag);
		FLAG_SUMMARY(bool_flag);
		FLAG_SUMMARY(int_flag);
		FLAG_SUMMARY(multi_flag);

		cout << "\n";
		cout << "testcmd: " << (int)testcmd_arg.result.front() << "\n";
		cout << "testcmdinner: " << (int)testcmdinner_arg.result.front() << "\n";

	} else if (help_flag) {
		cout << "Arguments:\n";
		args::print_help(arg_parser, {"  ", 4}, cout);
		cout << '\n';

		cout << "Flags:\n";
		args::print_help(flag_parser, {"  ", 4}, cout);

	} else {
		args::report_error(result, cout);
	}
}
