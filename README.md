Args
====

This is a C++ argument parser library.

It's type-safe and it doesn't use strings or similar techniques to retrieve
results.  Arguments are represented by actual objects which are passed by
lvalue-reference to the parser.

There are a few pre-made arguments (`LambdaArg`, `MapLookupArg`, `Flag`) and
parsers (`ArgParser`, `FlagParser`) for common usage, but it's completely
extensible (see `LambdaArg` and `FlagParser` respectively for easy examples).

A challenge has been to avoid dynamic dispatch completely and I think I almost
achieved it except for the `to_str` method, which is used for error reporting.

Examples
=======

Conditions are in the namespace `args::condition` which is not specified for
brevity. I recommend putting `using namespace args::condition` in the function
that declares and uses parsers. The source for this example can be found at
`readme_example.cpp` and compiled with `make example` (you will need to build
the library first with `make libargs`)

Argument declarations:

* Flag that expects no parameters. Shortopt is `-f` and longopt is `--flag`:
```c++
    auto flag = args::flag('f', "flag")
        .description("a flag"); // description to show in the help message
```

* Flag that expects an integer. Has no shortopt and longopt is `--integer`:
```c++
    auto integer_flag = args::flag<int>("integer")
        .metavers("INT") // parameter name to display in the help message
        .description("takes an integer");
```

* Requires `-f` or `--flag`:
```c++
    auto requires_flag = args::flag('r', "requires-flag", True(), requires(flag))
        .description("does nothing, but requires -f or --flag");
```

* Help flag. Note that `max(0)` is used as a postcondition so that -h is
  considered an error and will not report further errors. Postconditions are
  evaluated when all elements in argv have been parsed.
```c++
    auto help_flag = args::flag('h', "help", True(), max(0))
        .description("print this message and exit");
```

* Flag that must be specified at least once:
```c++
    auto once_flag = args::flag('o', "once", True(), min(1))
        .description("can only be specified once");
```

* Either `build` or `run`. It must appear exactly once. This will only be
  considered it has not been given yet. Preconditions are similar to this
  pseudocode:
  ` if precondition_ok() then parse(requires_flag); else try_next()`
  Note that `max(0)` is used instead of `max(1)` because the condition is
  evaluated before parsing, in which case it would have been found 0 times:
```c++
    enum class Command { BUILD, RUN };
    auto command = args::map_lookup_arg<Command>({
        { "build", Command::BUILD },
        { "run",   Command::RUN   }
    }, max(0), exactly(1))
        .description("available commands"); // shown in the generated help message
```

* Either `all` or `current`. It will only be considered after a `command` has
  been found and must appear exactly once:
```c++
    enum class Target { ALL, CURRENT };
    auto target = args::map_lookup_arg<Target>({
        { "all",     Target::ALL     },
        { "current", Target::CURRENT }
    }, all(after(command), max(0)), exactly(1))
        .description("available targets");
```

* Group them by priority. Note that `command` and `target` will fail to
  parse immediately and  args::parse()` will return an error. Flags should
  also be put in the first place in order to be considered whenever an argument
  starting with "-" is found. The endopts flag ("--") is built-in and checked
  automatically.
```c++
    // flags require the flags() parser because they need extra logic for shortopts
    // and the endopts mark. anything else can be in a args() parser.
    // order in which they are specified is the order in which the parser will
    // try to parse them.
    auto flag_parser = args::flags(flag, integer_flag, requires_flag, help_flag, once_flag);
    auto argument_parser = args::args(command, target);
```

* Parsing argv:
```c++
    // takes two InputIterators (args_begin and args_end). value_type must be
    // convertible to args::StrView (const char* or std::string)
    auto result = args::parse(argv+1, argv+argc, flag_parser, argument_parser);
```

* Using the result:
```c++
    using std::cout;
    using std::boolalpha;

    if (result.is_ok()) {
        cout << "Flag has been given " << flag.multiplicity << " times\n";

        cout << "Found --integer values: ";

        // .result is a vector<T>. In the case of flags, T is a tuple
        // of the parameter types, in this case, tuple<int>
        for (const auto& i : integer_flag.result)
            cout << std::get<0>(i) << ' ';

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
```

Another working example can be found in `main.cpp`, which I use for testing.

Documentation
===============

I hate writing documentation, so here it goes a brief explaination.

This library has two main components: arguments and parsers. Arguments describe
actual arguments that should be taken into account when parsing. They also
contain logic about how they should be parsed. Parsers just store references to
arguments with the same priority and loop through them.

There are also three functions that are provided: `parse`, `report_error` and
`print_help`. They are mostly for convenience, since `parse` will only initialize
an `OwnedParserState` and loop through `argv` and the provided parsers until
everything is matched or an error occurs. `report_error` will simply show an
easy to read message describing the provided error and `print_help` will
generate a common help message for the given parser with the specified `HelpFmt`.

All arguments derive from `Arg<T, Derived>`, where `T` is the type to be parsed
(e.g. an enum member for commands or a `Path` for an input file) and `Derived`
is the type itself for the CRTP. In order to provide an implementation for the
argument, the methods `ParseResult<T> parse_impl(ParserState&)` and
`virtual std::string to_str() const` have to be implemented. Although it's not
necessary, a helper factory function makes usage easier to read and write.
You can see `lambda_arg.hpp`, `map_lookup_arg.hpp` and `flag.hpp` for example
argument implementations. If you only need to use the argument once, check out
`LambdaArg` or `MapLookupArg`, they may be useful. Flags have a different
extension mechanism, you should specialize `ParamParser` for your type instead
of inheritance, `Flag`s will automatically be able parse your type with it.

Arguments also contain preconditions and postconditions. These are functors
that evaluate an argument with a `ParserState` and return either `success()`
or a `ParseError`. The difference between preconditions and postconditions is
that preconditions are evaluated before parsing and skip the argument in the
parser pass if they fail, whereas postconditions are evaluated after parsing
all arguments and the parser returns immediately. You can find pre-made
conditions in `conditions.hpp`.

Parsers are just containers for arguments and they are also extensible if it's
necessary to add special logic on top of arguments (see `FlagParser` in `flag.hpp`
for an example) also deriving from `ArgParser<ArgTs...>`, where `ArgTs` are the
types of the arguments to manage.
