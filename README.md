Args
====

This is a C++ argument parser library.

It's type-safe and it doesn't use strings or similar techniques retrieve
results.  Arguments are represented by actual objects which are passed by
lvalue-reference to the parser.

There are a few pre-made arguments (`LambdaArg`, `MapLookupArg`, `Flag`) and
parsers (`ArgParser`, `FlagParser`) for common usage, but it's completely
extensible (see `LambdaArg` and `FlagParser` for some easy examples).

A challenge has been to avoid dynamic dispatch completely and I think I almost
achieved it except for the `to_str` method, which is used for error reporting.

Example
=======

See `main.cpp` for an example program, you can compile with
`clang++ -std=c++11 main.cpp -I.` or `g++ -std=c++11 main.cpp -I.`. I haven't
tried with VS.
