#include "syntax_parser.h"
#include "jc_util.h"
#include "math_parser.h"
#include <stdlib.h>

/////////////////////////////////////////////////////////////
//                     Syntax Parser
/////////////////////////////////////////////////////////////

bool syntax_brackets_and_get_args(string_list_t * args, lexer_token_list_t * list)
{
    // Check that the brackets are syntactically correct. Opening bracket, 0 or more labels inside, and Ending bracket is valid

    if (list == NULL || list->tokens[list->index] == NULL)
    {
        set_global_err("Parser Error: A null string_list_t, lexer_token_list, or lexer_token_list pointing to a null token was passed to check_syntax_bracket_args check_syntax_bracket_args()");
        return false;
    }
    
    lexer_token_t * tok = list->tokens[list->index];

    if (tok->token_str[0] != '[')
    {
        setf_global_err("Input Error: Expected '[' but encountered %s", tok->token_str);
        return false;
    }
    else if (strlen(list->tokens[list->index]->token_str) != 1)
    {
        setf_global_err("Parser Error: Lexer failed to make '[' a single char. Encountered %s", tok->token_str);
        return false;
    }

    list->index++;
    tok = list->tokens[list->index];

    while (tok != NULL && tok->token_str[0] != ']')
    {
        if (tok->type != LEXER_TOKEN_T_LABEL)
        {
            setf_global_err("Input Error: Encountered %s inside brackets when a label was expected", tok->token_str);
            return false;
        }

        list->index++;
        tok = list->tokens[list->index];
    }

    if (tok == NULL || tok->token_str[0] != ']')
    {
        setf_global_err("Input Error: Closing brace not encountered after opening brace. Token before is %s", list->tokens[list->index - 1]->token_str);
        return false;
    }

    list->index++;

    return true;
}

bool syntax_parenned_expression(string_list_t * variables, lexer_token_list_t * list);

bool _syntax_function_check_def_and_label(lexer_token_list_t * list)
{
    lexer_token_t * tok = list->tokens[list->index];

    if (tok->type != LEXER_TOKEN_T_KEYWORD && strcmp(tok->token_str, "def") != 0)
    {
        setf_global_err("Input Error: def not encountered when function was expected. Encountered %s", tok->token_str);
        return false;
    }

    if (list->index + 3 >= list->size)
    {
        set_global_err("Input Error: Not enough tokens after def to create a variable or function");
        return false;
    }

    list->index++;
    tok = list->tokens[list->index];
    
    if (tok->type != LEXER_TOKEN_T_LABEL)
    {
            setf_global_err("Error: %s after def is not a label", tok->token_str);
            return -424242;
    }
    
    list->index++;

    return true;
}

bool syntax_end_paren_or_statement(lexer_token_list_t * list)
{
    lexer_token_t * tok = list->tokens[list->index];

    if (tok == NULL) // end of the statement
        return true;
    else if (tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0])
    {
        list->index++;
        return true;
    }
    //else if (tok->type == LEXER_TOKEN_T_STATEMENT_END)

    return false;
}

bool syntax_label_def(string_list_t * variables, lexer_token_list_t * list)
{
    if (list == NULL)
    {
        set_global_err("Parser Error: Null pointer passed to syntax_function()");
        return false;
    }

    if (_syntax_function_check_def_and_label(list) == false)
        return false;

    if (list->index >= list->size)
    {
        set_global_err("Input Error: Defined label, but did not assign any value to it");
        return false;
    }


    lexer_token_t * tok = list->tokens[list->index];
    // label is a function
    if (tok->type == LEXER_TOKEN_T_BRACKET)
    {
        string_list_t * new_function_args = string_list_t_new();

        if ( syntax_brackets_and_get_args(new_function_args, list) == false )
            return false;
        if ( syntax_parenned_expression(new_function_args, list) == false )
            return false;
    }
    // label is a variable
    else if (tok->type == LEXER_TOKEN_T_NUMBER || tok->type == LEXER_TOKEN_T_PAREN) // TODO: create syntax_expression() for either number or parenthases
    {
        if ( syntax_number(variables, list) == false )
            return false;
    }

    return true;
}

bool syntax_matop(string_list_t * variables, lexer_token_list_t * list)
{
    lexer_token_t * check_tok = list->tokens[list->index - 1];

    if (check_tok->type != LEXER_TOKEN_T_NUMBER)
    {
        setf_global_err("Input Error: Expected a number before an operator. Encountered %s", check_tok->token_str);
        return false;
    }
            
    check_tok = list->tokens[list->index];
    if (check_tok->type != LEXER_TOKEN_T_MATOP)
    {
        setf_global_err("Parser Error: Expected a mathmatical operator, but encoutnered %s", check_tok->token_str);
        return false;
    }

    check_tok = list->tokens[list->index + 1];
    list->index++;
    if (check_tok == NULL)
    {
        setf_global_err("Input Error: Expected a token after operator %s, but nothing was encountered", list->tokens[list->index]->token_str);
        return false;
    }
    else if (check_tok->type == LEXER_TOKEN_T_NUMBER || (check_tok->type == LEXER_TOKEN_T_PAREN && check_tok->token_str[0] == '('))
    {
        list->index++;
        return true;
    }
    else
    {
        setf_global_err("Input Error: Expected a number or expression after operator. Encountered %s", check_tok->token_str);
        return false;
    }
}

bool syntax_number(string_list_t * variables, lexer_token_list_t * list)
{
    if (list == NULL)
    {
        set_global_err("Input Error: A null list pointer was passed into syntax_number()");
        return false;
    }

    lexer_token_t * tok = list->tokens[list->index];
    if (tok->type == LEXER_TOKEN_T_LABEL)
    {
        // TODO: Check for distinction between functions and variables. Just a function name shouldn't work
        return string_list_t_contains(variables, tok->token_str);
    }
    else if (tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0] == '(')
    {
        return syntax_parenned_expression(variables, list);
    }
    else if (tok->type != LEXER_TOKEN_T_NUMBER)
    {
        setf_global_err("Parser Error: A non number token was encountered when a number was expected. Encountered %s", tok->token_str);
        return false;
    }
    


    list->index++;
    tok = list->tokens[list->index];

    // NULL or ')' indicate the end of an expresssion. A single number is a valid expression
    if (tok == NULL)
        return true;
    else if (tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0] == ')')
        return true;
    else if (tok->type == LEXER_TOKEN_T_MATOP)
    {
        return syntax_matop(variables, list);
    }
    else
    {
        setf_global_err("Input Error: Encountered invalid token after number. Should have encountered end of expression, ')', or an operator. Encoutnered %s", tok->token_str);
        return false;
    }
}

 bool syntax_keyword(string_list_t * functions, lexer_token_list_t * list)
 {
     lexer_token_t * tok = list->tokens[list->index];

     if ( strcmp(tok->token_str, "def") == 0 )
    {
        if ( syntax_label_def(functions, list) == false ) // label can correspond to either function or variable (which is a 0 argument function)
            return false;
        else
            return true;
    }

    return false;
}

bool syntax_parenned_expression(string_list_t * functions, lexer_token_list_t * list)
{
    if (list == NULL)
    {
        set_global_err("Parser Error: A null lexer_token_list_t pointer was passed into syntax_expression()");
        return false;
    }

    lexer_token_t * tok = list->tokens[list->index];

    if (tok == NULL)
    {
        set_global_err("Input Error: No token encountered when '(' to start an expression was expected");
        return false;
    }

    list->index++;
    tok = list->tokens[list->index];

    while (tok != NULL && tok->token_str[0] != ')')
    {
        // nested parenthase'd expression
        switch (tok->type)
        {
        case LEXER_TOKEN_T_PAREN:
            if (syntax_parenned_expression(functions, list) == false)
                return false;
            break;
        case LEXER_TOKEN_T_LABEL:
            if (syntax_number(functions, list) == false)
                return false;
            break;
        case LEXER_TOKEN_T_BRACKET:
            set_global_err("Input Error: Encountered bracket inside a parenthase'd expression. Did not follow the correct rules of function definition");
            return false;
        case LEXER_TOKEN_T_MATOP:
            if (syntax_matop(functions, list) == false)
                return false;
            break;
        case LEXER_TOKEN_T_KEYWORD:
            if (syntax_keyword(functions, list) == false)
                return false;
            break;
        case LEXER_TOKEN_T_NUMBER:
            if (syntax_number(functions, list) == false)
                return false;
            break;
        }

        tok = list->tokens[list->index];
    }

    if (tok == NULL)
        return false;
    else if (tok->token_str[0] == ')')
        return true;
    else
        return false;
}


int check_syntactically_correct(lexer_token_list_t * list)
{
    if (list == NULL)
        return 0;

    // we go into functions for checking the validity of different things. To make things easier, and not check at 50 different points
    // for invalid tokens, we do an invalid token check at the start on every token
    // TODO: move this responsibility onto the lexer
    for (list->index = 0; list->index < list->size; list->index++)
    {
        lexer_token_t * tok = list->tokens[list->index];
        if (tok->type == LEXER_TOKEN_T_INVALID_TOKEN)
        {
            set_global_err("Error, encountered invalid token");
            return -424242;
        }
    }
    
    string_list_t * seen_variables = string_list_t_new();

    // check validity of different language constructs
    for (list->index = 0; list->index < list->size; list->index++)
    {
        lexer_token_t * tok = list->tokens[list->index];

        switch (tok->type)
        {
            case LEXER_TOKEN_T_NUMBER:
            {
                if (syntax_number(seen_variables, list) == false)
                    return -424242;
                break;
            }
            case LEXER_TOKEN_T_MATOP:
            {
                if ( syntax_matop(seen_variables, list) == false )
                    return -424242;
                break;
            }
            case LEXER_TOKEN_T_BRACKET:
            {
                
                // this is where you would encounter brackets being used to pass arguments into a function
                // it's at this point that I've decided this syntax_parser needs to be reworked. I'll start with a grammar for it

                //// go back to the beginning of the function definition (the named label before opening bracket) for the syntax function to work properly
                //list->index--;
                //if ( syntax_function(list) == false )
                //    return -424242;
                //break;
            }
            case LEXER_TOKEN_T_PAREN:
            {
                if ( syntax_parenned_expression(seen_variables, list) == false )
                    return -424242;
                if ( list->tokens[list->index] == NULL || list->tokens[list->index]->type != LEXER_TOKEN_T_PAREN || list->tokens[list->index]->token_str[0] != ')' )
                    return -424242;

                list->index++;
                break;
            }
            case LEXER_TOKEN_T_KEYWORD:
            {
                if ( syntax_keyword(seen_variables, list) == false )
                    return -424242;
                break;
            }
        }

    }

    return 0;
}






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
    lexer_token_t * tok = list->tokens[list->index];

    return ( strcmp(tok->token_str, "def") == 0 );
}


bool get_next_token(lexer_token_t * change, lexer_token_list_t * list)
{
    list->index++;
    change = list->tokens[list->index];
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

bool syntax_def_type_label(func_list_t * seen_functions, lexer_token_list_t * list)
{
    if ( list == NULL ) { return false; }

    lexer_token_t * tok = list->tokens[list->index];
    if ( tok == NULL ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_KEYWORD || strcmp(tok->token_str, "def") != 0)
        return false;

    get_next_token(tok, list);
    if ( tok == NULL ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_TYPE || strcmp(tok->token_str, "int") != 0 )
        return false;

    get_next_token(tok, list);
    if ( tok == NULL ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_LABEL )
        return false;

    list->index++;
    return true;
}





string_list_t * syntax_bracket_and_num_args(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if ( list == NULL ) { return false; }

    lexer_token_t * tok = list->tokens[list->index];
    if ( tok == NULL ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_BRACKET || tok->token_str[0] != '[' )
        return NULL;
    
    // for loop collects the names of arguments
    // function arguments exist at different scope from our seen variables
    string_list_t * arguments = string_list_t_new();
    do {
        get_next_token(tok, list);
        if ( tok == NULL ) { return false; }

        string_list_t_push_copy(arguments, tok->token_str);
    } while ( tok->type != LEXER_TOKEN_T_LABEL);


    if ( tok->type != LEXER_TOKEN_T_BRACKET || tok->token_str[0] != ']' )
        return NULL;

    list->index++;
    return arguments;
}



int syntax_func_def_num_args(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if ( syntax_def_type_label(seen_funcs, list) == false )
        return -1;

    string_list_t * arguments = syntax_bracket_and_num_args(seen_funcs, list);

    // TODO:
    // check for a single_val or a parenned expression
    return -1;
}


bool syntax_operator_func_call(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    if ( ! func_list_t_contains(seen_funcs, list->tokens[list->index]->token_str) )
        return false;
    
    // an operator function is just a function which takes two arguments
    //return syntax_func_def_num_args(seen_funcs, list) == 2;
    // todo: similar function to the above, except for calling instead of defining, that way we can check that we have seen the labels we are passing in
    return false;
}

bool syntax_func_call(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! get_next_token(tok, list) ) { return false; }

    if ( tok->type != LEXER_TOKEN_T_LABEL || ! func_list_t_contains(seen_funcs, tok->token_str) )
        return false;


    func_t * func_to_call = func_list_t_find(seen_funcs, tok->token_str);
    if ( func_to_call == NULL ) { return false; }

    // no arguments needed if a function takes no arguments
    if ( (! get_next_token(tok, list)) && func_to_call->num_args == 0 )
        return true;

    // check for arguments to pass to the function
    string_list_t * args = syntax_bracket_and_num_args(seen_funcs, list);
    if (args == NULL) { return false; }
    for (int i = 0; i < args->size; i++)
    {
        func_t * pass_as_arg = func_list_t_find(seen_funcs, args->strings[i]);
        if (pass_as_arg == NULL || pass_as_arg->num_args != 0) // TODO: Allow for passing functions taking arguments as parameters
            return false;
    }

    free(args);
    return true;
}


bool syntax_func_call_or_def(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! get_next_token(tok, list) ) { return false; }

    if ( syntax_def_keyword(seen_funcs, list) )
    {
        list->index--;
        return syntax_func_def_num_args(seen_funcs, list) != -1;
    }

    return syntax_func_call(seen_funcs, list);
}


bool second_syntax_parenned_expression(func_list_t * seen_funcs, lexer_token_list_t * list)
{
    lexer_token_t * tok;
    if ( ! get_next_token(tok, list) ) { return false; }

    if ( tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0] == '(' )
        return second_syntax_parenned_expression(seen_funcs, list);

    return syntax_func_call_or_def(seen_funcs, list);
    
    
    list->index++;
    return ( tok->type == LEXER_TOKEN_T_PAREN && tok->token_str[0] == ')' );
}


int check_syntactically_correct_funcs(lexer_token_list_t * list)
{
    if (list == NULL || list->index >= list->size)
        return 0;
    
    func_list_t * seen_funcs = func_list_t_new();

    // check validity of different language constructs
    for (list->index = 0; list->index < list->size; list->index++)
    {
        lexer_token_t * tok = list->tokens[list->index];

        switch (tok->type)
        {
            case LEXER_TOKEN_T_KEYWORD:
                if ( ! syntax_def_keyword(seen_funcs, list) )
                    return -424242;
        }

    }

    return 0;
}







