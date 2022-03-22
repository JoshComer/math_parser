# josh
Josh's Own SHell

A shell for linux written by me, Josh. I really hate bash's syntax. As of right now, it's very simple, but I hope to eventually be able to replace bash with this for my own uses.

The shell can execute commands with arguments (the size of a line it can process is currently limited to 1024 chars)

~ will be substituted with the HOME env var
single quotes encapsulate string literals with no substitution (except single quotes of course, those must be escaped with \'. In the future maybe like bash strings next to each other will be concatenated)

TODO: Add history which is accessible through history command or up arrow - as well as ctrl+l for clearing screen
    will require changing the way input is gotten


chars which must be escaped: (){}[]=!~#