#include "math_parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

lexer_token_t * lexer_token_t_new_empty()
{
	lexer_token_t * token = malloc(sizeof(lexer_token_t));
	if (token == NULL)
		return NULL;

	token->token_str = NULL;

	token->token_str_len = -1;

	token->type = LEXER_TOKEN_T_INVALID_TOKEN;
	token->_must_free = true;

	return token;
}

void lexer_token_t_free(lexer_token_t *token)
{
	if (token == NULL)
		return;

	if (token->token_str != NULL)
		free(token->token_str);

	if (token->_must_free)
		free(token);
}


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

void lexer_token_t_set_type(lexer_token_t * token, LEXER_TOKEN_T_TYPES type)
{
	if (token == NULL)
		return;

	if (type <= _LEXER_TOKEN_T_TYPE_LOWER_BOUND && _LEXER_TOKEN_T_TYPE_UPPER_BOUND <= type)
		token->type = LEXER_TOKEN_T_INVALID_TOKEN;
	else
		token->type = type;
}


void lexer_token_list_t_print_all(lexer_token_list_t * list)
{
	if (list == NULL)
		return;

	for (int i = 0; i < list->size; i++)
		printf("%d:%d:%s\n", i, list->tokens[i]->type, list->tokens[i]->token_str);
}


ast_node_t * ast_node_t_new_empty()
{
	ast_node_t * node = malloc(sizeof(ast_node_t));
	if (node == NULL)
		return NULL;

	node->parent = NULL;
	node->l_child = NULL;
	node->r_child =NULL;

	node->str = NULL;

	node->operation = JOSH_INVALID_OPER;
	node->type = AST_NODE_T_TYPE_INVALID_TYPE;

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
		case AST_NODE_T_TYPE_KEYWORD:
			strcpy(type_str, "AST_NODE_T_TYPE_KEYWORD");
			break;
		case AST_NODE_T_TYPE_STRING_LITERAL:
			strcpy(type_str, "AST_NODE_T_TYPE_STRING_LITERAL");
			break;
		case AST_NODE_T_TYPE_TEXT:
			strcpy(type_str, "AST_NODE_T_TYPE_TEXT");
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


ast_node_t * ast_node_t_new(AST_NODE_T_TYPE type, JOSH_OPERATIONS operation, char * str)
{
	ast_node_t * node = ast_node_t_new_empty();
	if (node == NULL)
		return NULL;
	
	node->type = type;
	node->operation = operation;
    
    if (operation == JOSH_OPER_ADD || operation == JOSH_OPER_SUB)
        node->precedence = AST_PRECEDENCE_ADD_SUB;
    else if (operation == JOSH_OPER_MUL || operation == JOSH_OPER_DIV || operation == JOSH_OPER_MOD)
        node->precedence = AST_PRECEDENCE_MUL_DIV_MOD;
    else
    {
        node->precedence = AST_PRECEDENCE_INVALID;
    }

	node->str = malloc(strlen(str) + 1);
	
	if (node->str == NULL)
		return NULL;
	
	strcpy(node->str, str);

	return node;
}


ast_node_t * parse_matop(char * str)
{
	if (str == NULL)
		return NULL;

    //printf("Converting matop %s\n", str);

	switch (str[0])
	{
		case '+':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, JOSH_OPER_ADD, str);
		case '-':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, JOSH_OPER_SUB, str);
		case '*':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, JOSH_OPER_MUL, str);
		case '/':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, JOSH_OPER_DIV, str);
		case '%':
			return ast_node_t_new(AST_NODE_T_TYPE_MATOP, JOSH_OPER_MOD, str);
        case '^':
            return ast_node_t_new(AST_NODE_T_TYPE_MATOP, JOSH_OPER_EXP, str);
	}
		
    return NULL;
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
	token->_must_free = true;

	return token;
}


bool lexer_token_t_move(lexer_token_t * dest, lexer_token_t * src)
{
    if (src == NULL || dest == NULL || dest->token_str != NULL)
        return false;

    dest->token_str = malloc(strlen(src->token_str) * sizeof(char) + sizeof(char));
    if (dest->token_str == NULL) { free(dest); return NULL; }

    dest->token_str_len = src->token_str_len;
    dest->type = src->type;
    dest->_must_free = src->_must_free;
    
    strcpy(dest->token_str, src->token_str);
    free(src);

    return true;
}


bool lexer_token_list_t_push_and_move_token(lexer_token_list_t * list, lexer_token_t * token)
{
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


lexer_token_t * _parse_token(char * tok)
{
    //printf("bruh the stack is %s\n", tok);
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
            lexer_token_list_t_push_and_move_token(list, _parse_token(token_stack.buffer));
            jc_stack_t_reset(&token_stack);
        }
        else if (char_in_list(str[i], SEPARATORS, NUM_SEPARATORS))
        {
            // create a token with already collected chars. We can separate tokens on special chars without spaces
            if (token_stack.size != 0)
            {
                lexer_token_list_t_push_and_move_token(list, _parse_token(token_stack.buffer));
                jc_stack_t_reset(&token_stack);
            }

            jc_stack_t_push(&token_stack, str[i]);
            lexer_token_list_t_push_and_move_token(list, _parse_token(token_stack.buffer));
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
        lexer_token_list_t_push_and_move_token(list, _parse_token(token_stack.buffer));
    }

    if (list->size > 0)
        list->index = 0;

    return list;
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

ast_node_t * _parse_tokens_to_ast_tree(lexer_token_list_t * list);
ast_node_t * _parse_token_to_ast_node(lexer_token_list_t * list)
{
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
            return ast_node_t_new(AST_NODE_T_TYPE_NUMBER, JOSH_INVALID_OPER, check_token->token_str);
        case LEXER_TOKEN_T_MATOP:
            return parse_matop(check_token->token_str);
    }

    return NULL;
}


ast_node_t * _parse_tokens_to_ast_tree(lexer_token_list_t * list)
{
	if (list == NULL || list->size <= 0 || list->index < 0 || list->index > list->size)
		return NULL;
    
    //printf("List:%d:%d:%s\n", list->size, list->index, list->tokens[list->index]->token_str);
    
    // Set up the initial operations to start our tree
    // Loop is for any additional operations
    // anchor is the head of the previous node trio which was created
    ast_node_t * temp = _parse_token_to_ast_node(list);
    ast_node_t * anchor = _parse_token_to_ast_node(list);
    
    if (anchor == NULL)
        return temp;
    
    anchor->l_child = temp;
    temp->parent = anchor;
    
    anchor->r_child = _parse_token_to_ast_node(list);
    anchor->r_child->parent = anchor;

	while (list->index < list->size)
	{
        ast_node_t * op = _parse_token_to_ast_node(list);
        if (op == NULL) // right parenthasis encountered. Go back up a recursion level
            return anchor;
        //printf("Looping. Op is %s\n", op->str);

        op->r_child = _parse_token_to_ast_node(list);
        op->r_child->parent = op;

        //printf("new op is %s\n", new_op->str);
        if (op->precedence > anchor->precedence)
        {
            // the result of op takes the place of anchor->r_child
            // anchor instead uses the previous num from op->r_child as one operand
            op->l_child = anchor->r_child;
            op->l_child->parent = op;
            
            anchor->r_child = op;
            op->parent = anchor;
            
            anchor = op;
        }
        else
        {
            // Move up the tree to find where to insert the next operation.
            ast_node_t * replace = find_highest_node_with_precedence(anchor, op->precedence);
            // Whatever operation result would have been there will be the l_child
            // of the next operation
            //printf("replace is %s with l child of %s\n", replace->str, replace->l_child->str);
            if (replace->parent == NULL)
                op->parent = replace->parent;
            else if (replace->parent->l_child == replace)
            {
                op->parent = replace->parent;
                op->parent->l_child = op;
            }
            else
            {
                op->parent = replace->parent;
                op->parent->r_child = op;
            }

            op->l_child = replace;
            replace->parent = op;

            //printf("Added new r child after replacing. Is %s\n", new_op->r_child->str);
            anchor = op;
            //printf("l_child of anchor to head is %s\n", find_tree_head(anchor)->str);
        }
	}

	return find_tree_head(anchor);
}


int _math_eval_recurse(ast_node_t * node)
{
    if (node == NULL || node->type == AST_NODE_T_TYPE_INVALID_TYPE)
        return -424242;

    if (node->type == AST_NODE_T_TYPE_NUMBER)
        return strtoimax(node->str, NULL, 10);

    if (node->type == AST_NODE_T_TYPE_MATOP)
    {
        switch (node->operation)
        {
            case JOSH_OPER_ADD:
                return _math_eval_recurse(node->l_child) + _math_eval_recurse(node->r_child);
            case JOSH_OPER_SUB:
                return _math_eval_recurse(node->l_child) - _math_eval_recurse(node->r_child);
            case JOSH_OPER_MUL:
                return _math_eval_recurse(node->l_child) * _math_eval_recurse(node->r_child);
            case JOSH_OPER_DIV:
                return _math_eval_recurse(node->l_child) / _math_eval_recurse(node->r_child);
            case JOSH_OPER_MOD:
                return _math_eval_recurse(node->l_child) % _math_eval_recurse(node->r_child);
            case JOSH_OPER_EXP:
                return pow(_math_eval_recurse(node->l_child), _math_eval_recurse(node->r_child));
            default:
                return -424242;
        }
    }

    return -424242;
}


int math_eval(char * str)
{
    lexer_token_list_t * list = _tokenize_string(str);
    ast_node_t * tree_head = _parse_tokens_to_ast_tree(list);
    lexer_token_list_t_free(list);

    return _math_eval_recurse(tree_head);
}




