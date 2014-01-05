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

Example
=======

See `main.cpp` for an example program, you can compile it with
`make libargs test`.

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
`LambdaArg` or `MapLookupArg`, they may be useful.

Arguments also contain preconditions and postconditions. These are functors
that evaluate an argument with a `ParserState` and return either `success()`
or a `ParseError`. The difference between preconditions and postconditions is
that preconditions are evaluated before parsing and skip the argument in the
parser pass if they fail, whereas postconditions are evaluated after parsing
all arguments and the parser returns immediately. You can find pre-made
conditions in `conditions.hpp`.

Parsers are just containers for arguments and they are also extensible if it's
necessary to add some logic on top parsers (see `FlagParser` in `flag.hpp` for
an example) also deriving from `ArgParser<ArgTs...>`, where `ArgTs` are the types
of the arguments to manage.
