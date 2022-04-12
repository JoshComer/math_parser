# math_parser

A math parser which supports variables, arbitrarily sized integers, and high precision floating point (although the floating point support really sucks right now because floating point is hard).

# TODO
I'll add the ability to create functions, execute many of the TODO's scattered around the code, make some more tests, and clean up the code base. After that I don't anticipate on working on this more, although I may come back and extend it to be a lisp or lisp-like interpreter *shrug*.

# Dependencies
There's a dependency on GMP for integers of arbitrary size. For debian based distros you'll want to install libgmp10 to run the program, and libgmp3-dev for compiling. No info for other distros or OS's, but I'm sure it won't be that hard to figure out how to install GMP (or it might already be installed if on linux)

There's also a dependency on MPFR to ensure correct rounding on floating point numbers at the cost of extra speed

# Running
To compile, simply run 'make'
To run the tests located in the tests directory, run 'make tests' (though these tests aren't extremely comprehensive at the moment)
