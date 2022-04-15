#include "syntax_parser.h"
#include "jc_util.h"
#include "math_parser.h"
#include <stdlib.h>

/////////////////////////////////////////////////////////////
//                     Syntax Parser
/////////////////////////////////////////////////////////////

typedef struct func_t {
    char * name;
    int num_args;
    // enum for return type
} func_t;


typedef struct func_list_t {
    func_t ** funcs;
    int size;
    int _funcs_allocated_spots;
} func_list_t;


func_list_t * func_list_t_new()
{
	const int initial_size = 1024;

	func_list_t * ret_list = malloc(sizeof(func_list_t));
	if (ret_list == NULL)
		return NULL;

	ret_list->funcs = malloc(initial_size * sizeof(func_t *));

	if (ret_list->funcs == NULL)
	{
		free(ret_list);
		return NULL;
	}


	ret_list->size = 0;
	ret_list->_funcs_allocated_spots = initial_size;
	return ret_list;
}


void func_list_t_free(func_list_t * list)
{
	if (list == NULL)
		return;

	for (int i = 0; i < list->size; i++)
		free(list->funcs[i]);

	free(list->funcs);
}

bool func_list_t_contains(func_list_t * list, char * name)
{
	if (list == NULL)
		return false;

	for (int i = 0; i < list->size; i++)
	{
		if (strcmp(list->funcs[i]->name, name) == 0)
			return true;
	}

	return false;
}

func_t * func_list_t_find(func_list_t * list, char * name)
{
	if (list == NULL)
		return NULL;

	for (int i = 0; i < list->size; i++)
	{
		if (strcmp(list->funcs[i]->name, name) == 0)
			return list->funcs[i];
	}

	return NULL;
}

bool func_list_t_push_func(func_list_t * list, char * name, int num_args)
{
	if (list == NULL || name == NULL || num_args < 0 || list->size >= list->_funcs_allocated_spots)
		return false;

	int name_size = strlen(name);

	func_t * new_func = malloc(sizeof(func_t *));
    if ( new_func == NULL ) { return false; }

    new_func->name = malloc((name_size * sizeof(char)) + sizeof(char));
	if ( new_func->name == NULL )
    {
        free(new_func);
		return false;
    }

	strcpy(new_func->name, name);

    list->funcs[list->size] = new_func;
	list->size++;

	return true;
}



bool syntax_def_keyword(func_list_t * functions, lexer_token_list_t * list)
{
    if (list->index >= list->size)
        return false;

    lexer_token_t * tok = list->tokens[list->index];

    return ( strcmp(tok->token_str, "def") == 0 );
}


bool pop_next_token(lexer_token_t ** change, lexer_token_list_t * list)
{
    if (change == NULL || list->index >= list->size)
        return false;

    *change = list->tokens[list->index];
    list->index++;
    return change != NULL;
}


bool peek_next_token(lexer_token_t ** change, lexer_token_list_t * list)
{
    if (change == NULL || list->index >= list->size)
        return false;

    *change = list->tokens[list->index];
    return change != NULL;
}


bool syntax_def_and_type(func_list_t * seen_functions, lexer_token_list_t * list)
{
    if ( list == NULL ) { return false; }

    lexer_token_t * tok;
    pop_next_token(&tok, list);
    if ( tok == NULL ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_KEYWORD || strcmp(tok->token_str, "def") != 0)
        return false;

    pop_next_token(&tok, list);
    if ( tok == NULL ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_TYPE || strcmp(tok->token_str, "int") != 0 )
        return false;

    return true;
}


bool syntax_bracket_and_num_args(func_list_t * arguments, func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if ( list == NULL ) { return false; }

    lexer_token_t * tok;
    if ( ! peek_next_token(&tok, list) ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_BRACKET || tok->token_str[0] != '[' )
        return false;
    
    // for loop collects the names of arguments
    // function arguments exist at different scope from our seen variables
    // TODO: allow for passing of functions which take arguments as parameters
    do {
        pop_next_token(&tok, list);
        if ( tok == NULL ) { return false; }

        

        func_list_t_push_func(arguments, tok->token_str, 0);
    } while ( tok->type != LEXER_TOKEN_T_LABEL);


    if ( tok->type != LEXER_TOKEN_T_BRACKET || tok->token_str[0] != ']' )
        return false;

    list->index++;
    return true;
}


bool syntax_parens(func_list_t * seen_funcs, lexer_token_list_t * list);


bool syntax_expression(func_list_t * seen_funcs, lexer_token_list_t * list);
// takes ownership of not ownly the operator, but also the right hand operand as well, because it goes hand in hand with the operator being called
bool syntax_operator_func_call(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    // TODO: Find elegant way to merge the two possibilities for true into one set of instructions
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }

    if ( char_in_list(tok->token_str[0], "+-*/%", 5) )
        return syntax_expression(seen_funcs, list);

    // check that the label has been assigned a function which takes two arguments
    func_t * operator = func_list_t_find(seen_funcs, tok->token_str);
    if ( operator == NULL ) { return false; }
    if ( operator->num_args != 2 ) return false;
    
    return syntax_expression(seen_funcs, list);
    
    // todo: similar function to the above, except for calling instead of defining, that way we can check that we have seen the labels we are passing in
    //return false;
}

bool syntax_func_call(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }

    if ( tok->type == LEXER_TOKEN_T_NUMBER )
        return true;

    if ( tok->type != LEXER_TOKEN_T_LABEL || ! func_list_t_contains(seen_funcs, tok->token_str) )
        return false;

    func_t * func_to_call = func_list_t_find(seen_funcs, tok->token_str);
    if ( func_to_call == NULL ) { return false; }

    // A 0 argument function is a valid call even if there's nothing but the name to refer to it
    if ( ! pop_next_token(&tok, list) )
        return true;

    // There can also optionally be brackets
    if (tok->token_str[0] != '[')
    {
        list->index--;
        func_list_t * temp_local_funcs = func_list_t_new();
        if ( ! syntax_bracket_and_num_args(temp_local_funcs, seen_funcs, list) ) { return false; }
        int num_args = temp_local_funcs->size;
        func_list_t_free(temp_local_funcs);
        return num_args;
    }

    // error if this has been reached
    return -1;
}


bool single_val_and_expression_end(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }

    if ( tok->type == LEXER_TOKEN_T_NUMBER )
        goto find_expression_end;

    if ( tok->type == LEXER_TOKEN_T_LABEL )
    {
        func_t * called_func = func_list_t_find(seen_funcs, tok->token_str);
        if ( called_func == NULL || called_func->num_args != 0 )
            return false;
    }

    // an expression ends with an end parenthases or with the end of the expression
find_expression_end:
    if ( ! peek_next_token(&tok, list) ) { return true; }
    return (tok->token_str[0] == ')' || tok->token_str[0] == '}');
}


bool syntax_func_def(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if ( syntax_def_and_type(seen_funcs, list) == false )
        return false;

    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }
    if ( tok->type != LEXER_TOKEN_T_LABEL )
        return false;

    func_list_t * func_args = func_list_t_new();
    if ( ! syntax_bracket_and_num_args(func_args, seen_funcs, list) ) 
    {
        if ( func_args != NULL && func_args->size != 0 )
            return false;
        // // a func def without brackets is only allowed for 0 argument functions, or variables.

        // lexer_token_t * tok;
        // if ( ! pop_next_token(&tok, list) ) { return false; }
        // if ( tok->type != LEXER_TOKEN_T_LABEL )
        //     return false;

        // return single_val_and_expression_end(seen_funcs, list);
    }
    int num_args = func_args->size;

    if ( ! pop_next_token(&tok, list) ) { return false; }

    // Not having braces for a function definition is only allowed if there is only a single value immediately after the label
    if ( tok->type != LEXER_TOKEN_T_BRACE || tok->token_str[0] != '{')
    {
        if ( num_args != 0 )
            return false;

        list->index--;
        return single_val_and_expression_end(seen_funcs, list);
    }

    //if ( ! syntax_func_call_or_def(seen_funcs, list) )
    //    return false;
    if ( ! syntax_expression(seen_funcs, list) )
        return false;

    if ( ! pop_next_token(&tok, list) ) { return false; }
    if ( tok->type != LEXER_TOKEN_T_BRACE || tok->token_str[0] != '}')
        return false;

    return true;
}

bool syntax_parens(func_list_t * seen_funcs, lexer_token_list_t * list);
bool syntax_func_call_or_def(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! peek_next_token(&tok, list) ) { return false; }

    if (tok->type == LEXER_TOKEN_T_PAREN)
    {
        return syntax_parens(seen_funcs, list);
    }

    if ( syntax_def_keyword(seen_funcs, list) )
    {
        return syntax_func_def(seen_funcs, list);
    }

    return syntax_func_call(seen_funcs, list);
}


bool syntax_expression(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    // TODO: for now an empty expression is invalid. In the future it will be zero
    if (syntax_func_call_or_def(seen_funcs, list) == false)
        return false;

    // while loop checks for either, ')' or NULL ending expression, or an operator function chaining the previous function with the next
    lexer_token_t * tok;
    while ( peek_next_token(&tok, list) )
    {
        if ( tok->token_str[0] == ')' || tok->token_str[0] == '}' )
            return true;

        // check for an operator function chained after this one (like the '+' operator)
        if (syntax_operator_func_call(seen_funcs, list) == false)
            return false;
    }

    // an expression which ends with no chained operators after it is valid
    return true;
}


bool syntax_parens(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }

    if (tok->type == LEXER_TOKEN_T_PAREN)
    {
        if (tok->token_str[0] != '(')
            return false;
        
        if ( ! syntax_expression(seen_funcs, list) ) { return false; }

        if ( ! pop_next_token(&tok, list) ) { return false; }
        return (tok != NULL && tok->token_str[0] == ')');
    }
    else
    {
        return syntax_expression(seen_funcs, list);
    }
}


int check_syntactically_correct(lexer_token_list_t * list)
{
    list->index = 0;

    if (list == NULL)
        return -1;
    
    func_list_t * seen_funcs = func_list_t_new();

    lexer_token_t * tok;
    if ( ! peek_next_token(&tok, list) )
        return -1; // TODO: change to 0 later when empty expressions are valid
    
    if ( tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0] == '(' )
    {
        return syntax_parens(seen_funcs, list) == true ? 0 : -1;
    }
    else {
        return syntax_expression(seen_funcs, list) == true ? 0 : -1;
    }

}







