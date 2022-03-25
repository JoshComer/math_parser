# math_parser

A simple math parser written in C which supports arbitrarily sized integers. It's based partially off of some parsing code I was writing for my own shell, so it's still a little messy in some places. My plan for this is simple

(Done) 1. Remove the old code irrelevent to this parser
2. Extend functionality and refine existing design
3. Either create a primitive programming language, or resume work on my shell
    
I don't anticipate working too much on this project specifically, but that could change

# Dependencies
Theres a dependency on GMP for integers of arbitrary size. For debian based distros you'll want to install libgmp10 to run the program, and libgmp3-dev for compiling. No info for other distros or OS's, but I'm sure it won't be that hard to figure out how to install GMP (or it might already be installed if on linux)

# Running
To compile, simply run 'make'
To run the tests located in the tests directory, run 'make tests'
