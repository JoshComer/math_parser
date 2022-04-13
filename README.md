# math_parser

A math parser which supports variables, arbitrarily sized integers, and high precision floating point (although the floating point support really sucks right now because floating point is hard).

# Parser Documentation
Commands:
    To see a history of entered commands, simply enter 'hist'
    To see a list of variables you've assigned (up to 30 spots) enter 'var_print'

Variables:
    Valid variable names begin with a letter, and may contain letters or numbers
    
    To assign a number to a variable, type the desired variable name and the number value
        for example: 'newVar 8'
    You can also assign the results of expressions to variables by surrounding an expression in parenthases
        for example: 'newVar (4 + 5 / 3)

    To use a variable, just use it in place of a number. The following input is equivalent in the result (semicolon support for entering multiple statements simultaneously isn't added yet, but planned)
        '4 + 5' vs 'newVar 5; 4 + newVar'
        '6 ^ 8' vs 'six (6); eight 8; six ^ eight'

# TODO
1. Add an entry in the parsing pipeline between parser and ast generation that checks for correct syntax
2. Add functions, and the def keyword to define functions. Variables will now be functions with no arguments which return a value

I'll add the ability to create functions, execute many of the TODO's scattered around the code, make some more tests, and clean up the code base. After that I don't anticipate on working on this more, although I may come back and extend it to be a lisp or lisp-like interpreter *shrug*.

# Dependencies
There's a dependency on GMP for integers of arbitrary size. For debian based distros you'll want to install libgmp10 to run the program, and libgmp3-dev for compiling. No info for other distros or OS's, but I'm sure it won't be that hard to figure out how to install GMP (or it might already be installed if on linux)

There's also a dependency on MPFR to ensure correct rounding on floating point numbers at the cost of extra speed

# Running
To compile, simply run 'make'
To run the tests located in the tests directory, run 'make tests' (though these tests aren't extremely comprehensive at the moment)
