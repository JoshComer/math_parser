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




bool is_matop(lexer_token_t * tok)
{
    if (tok->type != LEXER_TOKEN_T_MATOP)
        return false;

    switch (tok->token_str[0])
    {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            return true;
        default:
            return false;
    }
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


bool is_num(func_list_t * seen_functions, lexer_token_list_t * list)
{
    lexer_token_t * tok = list->tokens[list->index];

    if ( tok == NULL )
        return false;

    return tok->type == LEXER_TOKEN_T_NUMBER;
}

bool is_seen_name_or_num(func_list_t * seen_functions, lexer_token_list_t * list)
{
    if ( list == NULL )
        return false;

    lexer_token_t * tok = list->tokens[list->index];

    if ( tok == NULL )
        return false;

    if (tok->type == LEXER_TOKEN_T_NUMBER)
        return true;
    else if (tok->type == LEXER_TOKEN_T_LABEL && func_list_t_contains(seen_functions, tok->token_str))
        return true;
    else
        return false;
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

    peek_next_token(&tok, list);
    if ( tok == NULL ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_LABEL )
        return false;

    return true;
}





bool syntax_bracket_and_num_args(func_list_t * arguments, func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if ( list == NULL ) { return false; }

    lexer_token_t * tok = list->tokens[list->index];
    if ( tok == NULL ) { return false; }

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


bool syntax_parenned_expression(func_list_t * seen_funcs, lexer_token_list_t * list);
int syntax_func_def_num_args(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if ( syntax_def_and_type(seen_funcs, list) == false )
        return -1;

    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return -1; }
    if ( tok->type != LEXER_TOKEN_T_LABEL )
        return -1;

    if ( ! peek_next_token(&tok, list) ) { return false; }

    func_list_t * new_func_locals = func_list_t_new();
    int num_arguments = 0;
    // a variable, or a 0 argument function with no argument brackets provided is allowed
    // this if statement checks for functions with arguments though, or a 0 argument function with empty brackets included
    if ( tok->token_str[0] == '[' )
    {
        if ( syntax_bracket_and_num_args(new_func_locals, seen_funcs, list) == false) { return false; }
        num_arguments = new_func_locals->size;
    }

    // passing in locals, because the only variables visible to the function are the arguments provided
    return syntax_parenned_expression(new_func_locals, list) ? num_arguments : -1;
}

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

bool syntax_func_call(func_list_t * seen_funcs, lexer_token_list_t * list);
bool syntax_n_specified_arg_func_call(int num_args, func_list_t * seen_funcs, lexer_token_list_t * list);
bool syntax_func_call_or_def(func_list_t * seen_funcs, lexer_token_list_t * list);
bool syntax_matop(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    // TODO: Add smaller table for predefined matops to determine if they've been reasigned
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }

    if ( tok->type == LEXER_TOKEN_T_MATOP )
    {
        if ( ! pop_next_token(&tok, list) ) { return false; }

        return syntax_func_call(seen_funcs, list);
    }

    if ( tok->type == LEXER_TOKEN_T_LABEL )
    {
        // parse the function for the operator ('+' in '8 + 7')
        list->index--;
        if ( syntax_n_specified_arg_func_call(2, seen_funcs, list) == false )
            return false;

        // parse the right hand operand to the operator
        if ( syntax_func_call_or_def(seen_funcs, list) == false )
            return false;
    }

    return false;
}

bool valid_matop_after_or_end(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if (list->index >= list->size)
        return true;

    return syntax_matop(seen_funcs, list);
}

bool syntax_n_specified_arg_func_call(int num_args, func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }

    if ( tok->type == LEXER_TOKEN_T_NUMBER )
        return true;

    if ( tok->type != LEXER_TOKEN_T_LABEL || ! func_list_t_contains(seen_funcs, tok->token_str) )
        return false;

    func_t * func_to_call = func_list_t_find(seen_funcs, tok->token_str);
    if ( func_to_call == NULL ) { return false; }
    if ( func_to_call->num_args != num_args )
        return false;

    // A 0 argument function is a valid call even if there's nothing but the name to refer to it
    if ( ! pop_next_token(&tok, list) )
    {
        return num_args == 0;
    }
    // There can also optionally be brackets
    if (tok->token_str[0] != '[')
    {
        list->index--;
        return true;
    }

    func_list_t * temp_local_funcs = func_list_t_new();
    if ( ! syntax_bracket_and_num_args(temp_local_funcs, seen_funcs, list) ) { return -1; }
    bool ret_val = temp_local_funcs->size == num_args;
    func_list_t_free(temp_local_funcs);

    return ret_val;
}

int syntax_n_arg_func_call(func_list_t * seen_funcs, lexer_token_list_t * list)
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

bool syntax_func_call(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }

    if ( tok->type == LEXER_TOKEN_T_NUMBER )
        return true;

    if ( tok->type != LEXER_TOKEN_T_LABEL || ! func_list_t_contains(seen_funcs, tok->token_str) )
        return false;

    return syntax_n_arg_func_call(seen_funcs, list) >= 0;
}


bool syntax_func_def(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if ( syntax_def_and_type(seen_funcs, list) == false)
        return false;
    
    int num_args = syntax_func_def_num_args(seen_funcs, list);
    if (num_args < 0)
        return false;

    // for a function definition, the expression must be surrounded by braces
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }
    if ( tok->type != LEXER_TOKEN_T_BRACE || tok->token_str[0] != '{')
        return false;

    if ( syntax_func_call_or_def(seen_funcs, list) != 0)
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
        return syntax_func_def_num_args(seen_funcs, list) != -1;
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
        if ( tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0] == ')' )
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
        if (tok->token_str[0] == '(')
            return false;
        
        syntax_expression(seen_funcs, list);

        return (tok != NULL && tok->token_str[0] == ')');
    }
    else
    {
        return syntax_expression(seen_funcs, list);
    }
}


bool syntax_parenned_expression(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! pop_next_token(&tok, list) ) { return false; }

    if ( tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0] == '(' )
        return syntax_parenned_expression(seen_funcs, list);

    return syntax_func_call_or_def(seen_funcs, list);
    
    
    list->index++;
    return ( tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0] == ')' );
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







