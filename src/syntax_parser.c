#include "syntax_parser.h"
#include "jc_util.h"
#include "math_parser.h"

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
