#include "math_parser.h"
#include <ctype.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <limits.h>


// ----------------------------------------
// Set global variables and error functions
// ----------------------------------------

#define _ERR_STR_BUFF_SIZE 101
int _NUM_ERRS = 0;
char _ERR_STR[_ERR_STR_BUFF_SIZE];

void set_global_err(char * err_str)
{
    if (err_str == NULL)
        return;

    _NUM_ERRS++;

    // Copy the error string over into global pointer
    strncpy(_ERR_STR, err_str, _ERR_STR_BUFF_SIZE - 1);
    _ERR_STR[_ERR_STR_BUFF_SIZE - 1] = '\0';
}

bool is_global_err()
{
    return _NUM_ERRS != 0;
}

void print_global_err()
{
    printf("%s\n", _ERR_STR);
}

void reset_global_err()
{
    _NUM_ERRS = 0;
}










//////////////////////////////////////////////
//       lexer_token_t type
//////////////////////////////////////////////

lexer_token_t * lexer_token_t_new_empty()
{
	lexer_token_t * token = malloc(sizeof(lexer_token_t));
	if (token == NULL)
		return NULL;

	token->token_str = NULL;

	token->token_str_len = -1;

	token->type = LEXER_TOKEN_T_INVALID_TOKEN;

	return token;
}


lexer_token_t * lexer_token_t_new(LEXER_TOKEN_T_TYPES type, char * str)
{
    if (str == NULL)
    {
        return NULL;
    }

	lexer_token_t * token = malloc(sizeof(lexer_token_t));
	if (token == NULL)
		return NULL;

	token->token_str = malloc((strlen(str) * sizeof(char)) + sizeof(char));
	if (token->token_str == NULL)
	{
		free(token);
		return NULL;
	}

	strcpy(token->token_str, str);
	token->token_str_len = strlen(str);

	token->type = type;

	return token;
}


void lexer_token_t_free(lexer_token_t *token)
{
	if (token == NULL)
		return;

	if (token->token_str != NULL)
		free(token->token_str);

    free(token);
}


bool lexer_token_t_move(lexer_token_t * dest, lexer_token_t * src)
{
    // Moves data AND ownership from src to data. Src is freed after
    // this function and should not be used. Dest can effectively be
    // used in src's place

    if (src == NULL || dest == NULL || dest->token_str != NULL)
        return false;

    // Create a valid dest to move data into
    dest->token_str = malloc(strlen(src->token_str) * sizeof(char) + sizeof(char));
    if (dest->token_str == NULL) 
    {
        free(dest);
        return false;
    }

    // actually move the data into dest
    dest->token_str_len = src->token_str_len;
    dest->type = src->type;
    strcpy(dest->token_str, src->token_str);
    
    free(src);

    return true;
}











//////////////////////////////////////////////
//       lexer_token_list_t type
//////////////////////////////////////////////

lexer_token_list_t * lexer_token_list_t_new()
{
	lexer_token_list_t * list = malloc(sizeof(lexer_token_list_t));
	if (list == NULL)
		return NULL;

	list->_allocated = _JOSH_TOKEN_LIST_T_SIZE;
	list->size = 0;

    for (int i = 0; i < _JOSH_TOKEN_LIST_T_SIZE; i++)
    {
        list->tokens[i] = NULL;
    }

    list->index = -1;

	return list;
}


void lexer_token_list_t_free(lexer_token_list_t * list)
{
	if (list == NULL)
		return;

	for (int i = 0; i < list->size; i++)
	{
		lexer_token_t_free(list->tokens[i]);
	}

}

lexer_token_t * lexer_token_list_t_pop(lexer_token_list_t * list)
{
	if (list->size <= 0)
		return NULL;

	list->size--;

    if (list->index >= list->size)
    {
        list->index --;
    }

	return list->tokens[list->size];
}


bool lexer_token_list_t_push_and_free_token(lexer_token_list_t * list, lexer_token_t * token)
{
    // Pushes data from token into list. Token is freed and should not be used afterwards

	if (list == NULL || token == NULL || list->size >= list->_allocated)
		return false;

	list->tokens[list->size] = lexer_token_t_new_empty();

	if ( ! lexer_token_t_move(list->tokens[list->size], token))
    {
        return false;
    }
    else
    {
        list->size++;
        return true;
    }
}


void lexer_token_list_t_print_all(lexer_token_list_t * list)
{
	if (list == NULL)
		return;

	for (int i = 0; i < list->size; i++)
		printf("%d:%d:%s\n", i, list->tokens[i]->type, list->tokens[i]->token_str);
}











//////////////////////////////////////////////
//            Lexing Functions
//////////////////////////////////////////////

lexer_token_t * _parse_token(char * tok)
{
    int length = strlen(tok);
    if (length == 1 && char_in_list(tok[0], "+-*/%^", 6))
    {
        return lexer_token_t_new(LEXER_TOKEN_T_MATOP, tok);
    }
    else if (length == 1 && char_in_list(tok[0], "()", 2))
    {
        return lexer_token_t_new(LEXER_TOKEN_T_PAREN, tok);
    }
    else
    {
        return lexer_token_t_new(LEXER_TOKEN_T_NUMBER, tok);
    }
}

lexer_token_list_t * _tokenize_string(char * str)
{
    const char * SEPARATORS  = "+-*/%^()";
    const int NUM_SEPARATORS = strlen(SEPARATORS);

    if (str == NULL)
        return NULL;

    lexer_token_list_t * list = lexer_token_list_t_new();
    jc_stack_t token_stack = jc_stack_t_new();

    for (int i = 0; i < (int)strlen(str); i++)
    {
        //printf("Looping. Size is %d with str of %s and char is %c\n", token_stack.size, token_stack.buffer, str[i]);
        if ( isspace(str[i]) && token_stack.size != 0)
        {
            lexer_token_list_t_push_and_free_token(list, _parse_token(token_stack.buffer));
            jc_stack_t_reset(&token_stack);
        }
        else if (char_in_list(str[i], SEPARATORS, NUM_SEPARATORS))
        {
            // create a token with already collected chars. We can separate tokens on special chars without spaces
            if (token_stack.size != 0)
            {
                lexer_token_list_t_push_and_free_token(list, _parse_token(token_stack.buffer));
                jc_stack_t_reset(&token_stack);
            }

            jc_stack_t_push(&token_stack, str[i]);
            lexer_token_list_t_push_and_free_token(list, _parse_token(token_stack.buffer));
            jc_stack_t_reset(&token_stack);
        }
        else
        {
            if ( ! isspace(str[i]) )
                jc_stack_t_push(&token_stack, str[i]);
        }
    }

    if (token_stack.size > 0)
    {
        lexer_token_list_t_push_and_free_token(list, _parse_token(token_stack.buffer));
    }

    if (list->size > 0)
        list->index = 0;

    return list;
}











//////////////////////////////////////////////
//             ast_node_t type
//////////////////////////////////////////////

ast_node_t * ast_node_t_new_empty()
{
	ast_node_t * node = malloc(sizeof(ast_node_t));
	if (node == NULL)
		return NULL;

	node->parent = NULL;
	node->l_child = NULL;
	node->r_child =NULL;

	node->str = NULL;

	node->operation = PARSER_INVALID_OPER;
	node->type = AST_NODE_T_TYPE_INVALID_TYPE;

	return node;
}


ast_node_t * ast_node_t_new(AST_NODE_T_TYPE type, PARSER_OPERATIONS operation, char * str)
{
    // create new ast_node_t
	ast_node_t * node = ast_node_t_new_empty();
	if (node == NULL)
		return NULL;
	
	node->str = malloc(strlen(str) + 1);
	if (node->str == NULL)
		return NULL;
	
    // copy data
	strcpy(node->str, str);
	node->type = type;
	node->operation = operation;
    
    // set precedence level for order of operations
    if (operation == PARSER_OPER_ADD || operation == PARSER_OPER_SUB)
        node->precedence = AST_PRECEDENCE_ADD_SUB;
    else if (operation == PARSER_OPER_MUL || operation == PARSER_OPER_DIV || operation == PARSER_OPER_MOD)
        node->precedence = AST_PRECEDENCE_MUL_DIV_MOD;
    else if (operation == PARSER_OPER_EXP)
        node->precedence = AST_PRECEDENCE_EXP;
    else
    {
        node->precedence = AST_PRECEDENCE_INVALID;
    }

	return node;
}


void ast_tree_free(ast_node_t * node)
{
	if (node == NULL)
		return;

	if (node->l_child != NULL)
		ast_tree_free(node->l_child);
	if (node->r_child != NULL)
		ast_tree_free(node->r_child);


	// base case => free all node data
	{
		if (node->str != NULL)
			free(node->str);
		
		free(node);
	}
}


void ast_node_t_set_l_child(ast_node_t * parent, ast_node_t * l_child)
{
    if (parent == NULL || l_child == NULL)
        return;

    parent->l_child = l_child;
    l_child->parent = parent;
}


void ast_node_t_set_r_child(ast_node_t * parent, ast_node_t * r_child)
{
    if (parent == NULL || r_child == NULL)
        return;

    parent->r_child = r_child;
    r_child->parent = parent;
}


ast_node_t * find_tree_head(ast_node_t * node)
{
    if (node == NULL)
        return node;

    while(node->parent != NULL)
        node = node->parent;

    return node;
}


ast_node_t * find_highest_node_with_precedence(ast_node_t * node, AST_PRECEDENCE precedence)
{
    // We look for the highest node in the tree with lesser or equal precedence to that passed in
    // Return either the highest node which matches our conditions, or return the head
    if (node == NULL)
        return NULL;

    while (node->parent != NULL && node->precedence > precedence)
    {
        node = node->parent;
    }

    return node;
}


void _print_ast_node(ast_node_t * node)
{
	char type_str[50];

	if (node == NULL)
	{
		printf("NULL\n");
		return;
	}

	// get type str
	switch (node->type)
	{
		case AST_NODE_T_TYPE_INVALID_TYPE:
			strcpy(type_str, "AST_NODE_T_TYPE_INVALID_TYPE");
			break;
		case AST_NODE_T_TYPES_NUM_TYPES:
			strcpy(type_str, "AST_NODE_T_TYPES_NUM_TYPES");
			break;
        case AST_NODE_T_TYPE_NUMBER:
            strcpy(type_str, "AST_NODE_T_TYPE_NUMBER");
            break;
        case AST_NODE_T_TYPE_MATOP:
            strcpy(type_str, "AST_NODE_T_TYPE_MATOP");
            break;
		default:
			printf("Invalid node type encountered when printing. Exiting\n");
			exit(-23478);
	}

	printf("Type:%s ", type_str);
	if (node->str != NULL)
		printf("Str:%s", node->str);

	printf("\n");
}

void _ast_tree_print_helper(ast_node_t * node, int depth, int num_spaces)
{
	for(int i = 0; i < depth *num_spaces; i++)
	{
		putchar(' ');
	}

	_print_ast_node(node);

	if (node->r_child != NULL)	
		_ast_tree_print_helper(node->r_child, depth + 1, num_spaces);
	if (node->l_child != NULL)
		_ast_tree_print_helper(node->l_child, depth + 1, num_spaces);
}

void ast_tree_print(ast_node_t * head)
{
	_ast_tree_print_helper(head, 0, 3);
}











//////////////////////////////////////////////
//         AST Generation Functions
//////////////////////////////////////////////

ast_node_t * parse_matop(char * str)
{
	if (str == NULL)
		return NULL;

	switch (str[0])
	{
		case '+':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, PARSER_OPER_ADD, str);
		case '-':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, PARSER_OPER_SUB, str);
		case '*':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, PARSER_OPER_MUL, str);
		case '/':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, PARSER_OPER_DIV, str);
		case '%':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, PARSER_OPER_MOD, str);
        case '^':
            return ast_node_t_new(AST_NODE_T_TYPE_MATOP, PARSER_OPER_EXP, str);
	}
		
    return NULL;
}


// prototype so the two functions can refer to each other
ast_node_t * _parse_tokens_to_ast_tree(lexer_token_list_t * list);
ast_node_t * _parse_token_to_ast_node(lexer_token_list_t * list)
{
    // parse a single lexer token into an ast node
    // in the case of parenthasis though, it's parsed
    // as a separate ast tree, and the head of the tree is returned

    if (list == NULL || list->size <= 0 || list->index < 0 || list->index >= list->size)
		return NULL;

    lexer_token_t * check_token = list->tokens[list->index];

    list->index++;

    switch (check_token->type)
    {
        case LEXER_TOKEN_T_PAREN:
            if (check_token->token_str[0] == '(')
                return _parse_tokens_to_ast_tree(list);
            else // Try to parse 
                return NULL;
        case LEXER_TOKEN_T_NUMBER:
            return ast_node_t_new(AST_NODE_T_TYPE_NUMBER, PARSER_INVALID_OPER, check_token->token_str);
        case LEXER_TOKEN_T_MATOP:
            return parse_matop(check_token->token_str);
    }

    return NULL;
}

ast_node_t * _parse_tokens_to_ast_tree(lexer_token_list_t * list)
{
	if (list == NULL || list->size <= 0 || list->index < 0 || list->index > list->size)
		return NULL;
    
    // Set up the initial operations to start our tree
    // anchor is the head of the previous node trio which was created
    ast_node_t * temp = _parse_token_to_ast_node(list);
    ast_node_t * anchor = _parse_token_to_ast_node(list);
    
    if (anchor == NULL) // End of parsing string or ')' was reached. Return everything gotten so far
        return temp;
    
    ast_node_t_set_l_child(anchor, temp);
    ast_node_t_set_r_child(anchor, _parse_token_to_ast_node(list));

	while (list->index < list->size)
	{
        ast_node_t * op = _parse_token_to_ast_node(list);
        if (op == NULL) // right parenthasis encountered. Go back up a recursion level
            return anchor;
        
        ast_node_t_set_r_child(op, _parse_token_to_ast_node(list));

        if (op->precedence > anchor->precedence)
        {
            // the result of op takes the place of anchor->r_child
            // anchor instead uses the previous num from op->r_child as one operand
            ast_node_t_set_l_child(op, anchor->r_child);
            ast_node_t_set_r_child(anchor, op);
            
            anchor = op;
        }
        else
        {
            // Move up the tree to find where to insert the next operation.
            ast_node_t * replace = find_highest_node_with_precedence(anchor, op->precedence);

            // move op into replace's position in the tree
            if (replace->parent == NULL)
                op->parent = NULL;
            else if (replace->parent->l_child == replace)
                ast_node_t_set_l_child(replace->parent, op);
            else
                ast_node_t_set_r_child(replace->parent, op);

            // Move replace underneath op as an operand to its operation
            ast_node_t_set_l_child(op, replace);

            anchor = op;
        }
	}

	return find_tree_head(anchor);
}











//////////////////////////////////////////////
//        Math Evaluation Functions
//////////////////////////////////////////////

bool _iof_convert_to_float(iof_num * integer)
{
    size_t num_digits = mpz_sizeinbase(integer->num.integer, 10);

    mpfr_t new_float;
    mpfr_init2(new_float, num_digits + 10);
    // Perform int to float conversion. Return if it can't EXACTLY represent the value in floating
    // todo: change this to allow for inexact conversion
    if (mpfr_set_z(new_float, integer->num.integer, MPFR_RNDN) != 0)
        return false;

    // switch integer to an iof_num representing the float result
    mpz_clear(integer->num.integer);
    mpfr_swap(integer->num.floating, new_float);
    integer->type = IOF_TYPE_FLOATING;

    return true;
}

// operand1_and_res - Used as an operand, and is where the result is stored
bool iof_add(iof_num * operand1_and_res, iof_num * operand2)
{
    // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        mpz_add(operand1_and_res->num.integer, operand1_and_res->num.integer, operand2->num.integer);
        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;

            mpfr_add(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            mpfr_add_z(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.integer, MPFR_RNDN);
            return true;
        }
        else {
            mpfr_add(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
    }
}

bool iof_subtract(iof_num * operand1_and_res, iof_num * operand2)
{
        // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        mpz_sub(operand1_and_res->num.integer, operand1_and_res->num.integer, operand2->num.integer);
        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;

            mpfr_sub(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            mpfr_sub_z(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.integer, MPFR_RNDN);
            return true;
        }
        else {
            mpfr_sub(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
    }
}

bool iof_multiply(iof_num * operand1_and_res, iof_num * operand2)
{
    // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        mpz_mul(operand1_and_res->num.integer, operand1_and_res->num.integer, operand2->num.integer);
        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;

            mpfr_mul(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            mpfr_mul_z(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.integer, MPFR_RNDN);
            return true;
        }
        else {
            mpfr_mul(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
    }
}

bool iof_divide(iof_num * operand1_and_res, iof_num * operand2)
{
    // convert ints to floating point, and return false for conversion error
    if (operand1_and_res->type == IOF_TYPE_INTEGER)
    {
        if (_iof_convert_to_float(operand1_and_res) == false)
            return false;
    }
    
    if (operand2->type == IOF_TYPE_INTEGER)
    {
        if (_iof_convert_to_float(operand2) == false)
            return false;
    }

    mpfr_div(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
    return true;
}

bool iof_modulus(iof_num * operand1_and_res, iof_num * operand2)
{
    // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        mpz_mod(operand1_and_res->num.integer, operand1_and_res->num.integer, operand2->num.integer);
        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand2) == false)
                return false;
        }

        mpfr_fmod(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
        return true;
    }
}

bool iof_exponentiation(iof_num * operand1_and_res, iof_num * operand2)
{
    // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        long exponent_power = mpz_get_si(operand2->num.integer);
        mpz_pow_ui(operand1_and_res->num.integer, operand1_and_res->num.integer, exponent_power);

        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand2) == false)
                return false;
        }

        mpfr_exp(operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
        return true;
    }
}

bool iof_cmp_si(iof_num * operand1, int operand2)
{
    if (operand1->type == IOF_TYPE_INTEGER)
    {
        return mpz_cmp_si(operand1->num.integer, operand2);
    }
    else {
        return mpfr_cmp_si(operand1->num.floating, operand2);
    }
}

bool iof_set_from_str(iof_num * result, char *string)
{
    bool is_float = false;
    for (int i = 0; i != '\0'; i++)
    {
        if (string[i] == '.')
        {
            is_float = true;
            break;
        }
    }

    if (is_float)
    {   // todo: add a check for if it's an int
        return mpfr_set_str(result->num.floating, string, 10, MPFR_RNDN);
    }
    else
    {   // todo: replace with a swap instead
        if (result->type == IOF_TYPE_FLOATING)
            iof_reinit_int(result);

        return mpz_set_str(result->num.integer, string, 10);
    }
}

void iof_init_int(iof_num * result)
{
    mpz_init(result->num.integer);
    result->type = IOF_TYPE_INTEGER;
    result->inited = true;
}

void iof_clear(iof_num * to_clear)
{
    if (to_clear->type == IOF_TYPE_INTEGER)
        mpz_clear(to_clear->num.integer);
    else
        mpfr_clear(to_clear->num.floating);

    to_clear->inited = false;
}

void iof_reinit_int(iof_num * to_reinit)
{
    if(to_reinit->inited == true)
        iof_clear(to_reinit);

    iof_init_int(to_reinit);
}

void iof_out_str(iof_num * num)
{
    if (num->type == IOF_TYPE_INTEGER)
        mpz_out_str(stdout, 10, num->num.integer);
    else
    {   // check if the floating point can be represented as an integer
        if (mpfr_integer_p(num->num.floating))
        {
            mpz_t temp;
            mpz_init(temp);
            mpfr_get_z(temp, num->num.floating, MPFR_RNDN);
            mpz_out_str(stdout, 10, temp);

            // TODO: The below line doesn't work for some reason. Figure out why, and replace the horrendous above lines with it
            //mpfr_printf("%Z", num->num.floating);
        }
        // TODO: fix printing for floating point to look like how a human would write it (not in exponentiation notation unless needed)
        else
            mpfr_out_str(stdout, 10, 0, num->num.floating, MPFR_RNDN);
    }
}

int _math_eval_recurse(iof_num * result, ast_node_t * node)
{
    if (node == NULL || node->type == AST_NODE_T_TYPE_INVALID_TYPE)
    {
        set_global_err("Error: Encountered a NULL node or INVALID AST NODE TYPE when math_eval_recursing");
        return -424242;
    }

    if (node->type == AST_NODE_T_TYPE_NUMBER)
    {
        // returns 0 if successful and -1 if error
        return iof_set_from_str(result, node->str);
    }
        //return strtoimax(node->str, NULL, 10); // TODO: Detect overflow or underflow

    if (node->type == AST_NODE_T_TYPE_MATOP)
    {
        //mpz_t operand_1; // for temporary values for calculation
        //mpz_init(operand_1); // TODO find a way to not reinit every time a calculation is done. Only one init'd variable is needed

        iof_num operand;
        iof_init_int(&operand);

        switch (node->operation)
        {   // TODO: Detect overflow or underflow
            case PARSER_OPER_ADD:
                _math_eval_recurse(result, node->l_child);
// above line used result instead of operand               mpz_set(operand_1, result);
                _math_eval_recurse(&operand, node->r_child); // result holds the second value we need

                iof_add(result, &operand);
                //mpz_add(result, operand_1, result); // return 0 if successful, -1 if error
                return 0;
                //return _math_eval_recurse(node->l_child) + _math_eval_recurse(node->r_child);
            case PARSER_OPER_SUB:
                _math_eval_recurse(result, node->l_child);
                //mpz_set(operand_1, result);
                _math_eval_recurse(&operand, node->r_child);

                iof_subtract(result, &operand);
                return 0;
            case PARSER_OPER_MUL:
                _math_eval_recurse(result, node->l_child);
                //mpz_set(operand_1, result);
                _math_eval_recurse(&operand, node->r_child);

                iof_multiply(result, &operand);
                return 0;
            case PARSER_OPER_DIV:
                _math_eval_recurse(result, node->l_child);
                //mpz_set(operand_1, result);
                _math_eval_recurse(&operand, node->r_child);

                if ( ! iof_cmp_si(&operand, 0))
                {
                    set_global_err("Error: Attempted divide by 0 encountered");
                    return -424242;
                }
                //else
                //{
                //    mpz_tdiv_q(result, operand_1, result);
                //    return 0;
                //}
                iof_divide(result, &operand);
                return 0;

            case PARSER_OPER_MOD:
                _math_eval_recurse(result, node->l_child);
                //mpz_set(operand_1, result);
                _math_eval_recurse(&operand, node->r_child);

                iof_modulus(result, &operand);
                return 0;
            case PARSER_OPER_EXP:
                _math_eval_recurse(result, node->l_child);
                //mpz_set(operand_1, result);
                _math_eval_recurse(&operand, node->r_child);

                //// if exponent fits inside a signed long
                //if (mpz_fits_slong_p(result))
                //{
                //    long exponent_power = mpz_get_si(result);
                //    mpz_pow_ui(result, operand_1, exponent_power);
                //    return 0;
                //}
                //else
                //{
                //    set_global_err("Error: Attempted to raise number to a power that doesn't fit inside signed long data type");
                //    return -1;
                //}

                iof_exponentiation(result, &operand);
                return 0;
                
            default:
                return -424242;
        }
    }

    return -424242;
}











////////////////////////////////////////////////

parser_history_t * parser_history_t_new()
{
    parser_history_t * hist = malloc(sizeof(parser_history_t));
    if (hist == NULL)
        return NULL;

    // each input_hist entry points to a different part of this giant malloc'd block
    hist->_str_pool_num_bytes = PARSER_HISTORY_SIZE * MATH_PARSER_INPUT_BUFF_SIZE * sizeof(char);
    hist->_str_pool = malloc(hist->_str_pool_num_bytes);
    if (hist->_str_pool == NULL)
    {
        free(hist);
        return NULL;
    }

    // initialize each hist_entry and point each entry's string to its spot in the _str_pool
    for (int i = 0; i < PARSER_HISTORY_SIZE; i++)
    {
        hist->hist_entries[i].input_str = hist->_str_pool + (i * MATH_PARSER_INPUT_BUFF_SIZE);
        hist->hist_entries[i].input_str[0] = '\0';
        hist->hist_entries[i].computed_result = -424242; // magic number
    }

    hist->_start_idx = 0;
    hist->_size_filled = 0;
    hist->last_entry_num = 0;

    return hist;
}

void parser_history_t_free(parser_history_t * hist)
{
    if (hist == NULL)
        return;

    if (hist->hist_entries != NULL)
        free(hist->_str_pool);

    free(hist);
}

bool parser_history_t_push(parser_history_t * hist, char * input_str, int computed_result)
{
    if (hist == NULL)
        return false;
    
    // circle around to the first of the array if we've grown past the array size. If this happens we overwrite oldest entry
    int hist_entry_index = hist->last_entry_num % PARSER_HISTORY_SIZE;

    // copy data into hist_entry
    hist->hist_entries[hist_entry_index].computed_result = computed_result;
    strncat(hist->hist_entries[hist_entry_index].input_str, input_str, MATH_PARSER_INPUT_BUFF_SIZE - 1);

    hist->last_entry_num++;
    
    // keep track of how many spots in our array have been used
    if (hist->_size_filled < PARSER_HISTORY_SIZE)
        hist->_size_filled++;

    // if we overwrote an entry set the start index to the spot past where we overwrote (the oldest entry)
    if (hist->last_entry_num > PARSER_HISTORY_SIZE)
        hist->_start_idx = (hist_entry_index + 1) % PARSER_HISTORY_SIZE;

    return true;
}


parser_hist_entry_t get_hist_entry_by_offset(parser_history_t * hist, int offset)
{
    // allows getting history items by negative offset. If 0 is entered, the last
    // history item is returned. -1 gets the item before that, so on and so forth
    // Out of range will return either the most recent or oldest entry

    if (hist == NULL || hist->_size_filled == 0)
    {
        parser_hist_entry_t empty = {NULL, -424242};
        return empty;
    }

    if (offset > 0) // return most recent entry if offset is above range
        return hist->hist_entries[(hist->last_entry_num - 1) % PARSER_HISTORY_SIZE];
    if (offset <= PARSER_HISTORY_SIZE * -1) // return oldest entry if offset is below range
        return hist->hist_entries[hist->_start_idx];

    return hist->hist_entries[(hist->last_entry_num + offset - 1) % PARSER_HISTORY_SIZE];
}

parser_hist_entry_t get_hist_entry_by_index(parser_history_t * hist, int hist_num)
{
    if (hist == NULL || hist->last_entry_num <= 0)
    {
        parser_hist_entry_t empty = {NULL, -424242};
        return empty;
    }

    // return latest history item if number is too big
    if (hist_num > hist->last_entry_num)
        return hist->hist_entries[hist->last_entry_num % PARSER_HISTORY_SIZE];
    if (hist_num < hist->last_entry_num - PARSER_HISTORY_SIZE)
        return hist->hist_entries[hist->_start_idx];
    
    return hist->hist_entries[(hist_num - 1) % PARSER_HISTORY_SIZE];
}





















int math_eval(iof_num * result, char * str)
{
    lexer_token_list_t * list = _tokenize_string(str);
    ast_node_t * tree_head = _parse_tokens_to_ast_tree(list);
    lexer_token_list_t_free(list);

    int error_val = _math_eval_recurse(result, tree_head);
    ast_tree_free(tree_head);

    return error_val;
}

