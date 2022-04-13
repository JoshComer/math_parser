#ifndef MATH_PARSER_SYNTAX_PARSER
#define MATH_PARSER_SYNTAX_PARSER

#include "math_parser.h"

int check_syntactically_correct(lexer_token_list_t * list);
bool syntax_number(string_list_t * variables, lexer_token_list_t * list);
bool syntax_matop(string_list_t * variables, lexer_token_list_t * list);


#endif