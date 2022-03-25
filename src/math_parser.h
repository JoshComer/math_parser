#ifndef JOSH_PARSER_HEADER
#define JOSH_PARSER_HEADER

#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>

#include "jc_util.h"

typedef enum LEXER_TOKEN_T_TYPES
{
	_LEXER_TOKEN_T_TYPE_LOWER_BOUND = -1,
	LEXER_TOKEN_T_INVALID_TOKEN,

    LEXER_TOKEN_T_MATOP,
    LEXER_TOKEN_T_NUMBER,
    LEXER_TOKEN_T_PAREN,

	LEXER_TOKEN_T_NUM_TOKEN_TYPES,
	_LEXER_TOKEN_T_TYPE_UPPER_BOUND,

} LEXER_TOKEN_T_TYPES;

typedef struct lexer_token_t{
	char * token_str;
	int token_str_len;

	int type;
} lexer_token_t;

lexer_token_t * lexer_token_t_new_empty();
lexer_token_t * lexer_token_t_new(LEXER_TOKEN_T_TYPES type, char * str);

bool lexer_token_t_move(lexer_token_t * dest, lexer_token_t * src);
void lexer_token_t_free(lexer_token_t * token);








#define _JOSH_TOKEN_LIST_T_SIZE 1024
typedef struct lexer_token_list_t{
	lexer_token_t * tokens[_JOSH_TOKEN_LIST_T_SIZE];

	int size;
	int _allocated;

    int index;
} lexer_token_list_t;

lexer_token_list_t * lexer_token_list_t_new();
void lexer_token_list_t_free(lexer_token_list_t * list);

bool lexer_token_list_t_push_and_free_token(lexer_token_list_t * list, lexer_token_t * token);
lexer_token_t * lexer_token_list_t_pop(lexer_token_list_t * list);

void lexer_token_list_t_print_all(lexer_token_list_t * list);







typedef enum AST_NODE_T_TYPE
{
	AST_NODE_T_TYPE_INVALID_TYPE = -1,

	AST_NODE_T_TYPE_MATOP,
    AST_NODE_T_TYPE_NUMBER,

	AST_NODE_T_TYPES_NUM_TYPES

} AST_NODE_T_TYPE;

typedef enum PARSER_OPERATIONS
{
	PARSER_INVALID_OPER,
	PARSER_OPER_MUL,
	PARSER_OPER_SUB,
	PARSER_OPER_ADD,
	PARSER_OPER_DIV,
	PARSER_OPER_MOD,
    PARSER_OPER_EXP

} PARSER_OPERATIONS;

typedef enum AST_PRECEDENCE
{
    AST_PRECEDENCE_INVALID,
    AST_PRECEDENCE_ADD_SUB,
    AST_PRECEDENCE_MUL_DIV_MOD,
    AST_PRECEDENCE_EXP,
    AST_PRECEDENCE_PAREN

} AST_PRECEDENCE;

typedef struct ast_node_t {
	struct ast_node_t * parent;

	struct ast_node_t * l_child;
	struct ast_node_t * r_child;

	char * str;

	AST_NODE_T_TYPE type;
	PARSER_OPERATIONS operation;
    AST_PRECEDENCE precedence;
} ast_node_t;

ast_node_t * ast_node_t_new_empty();
ast_node_t * ast_node_t_new(AST_NODE_T_TYPE type, PARSER_OPERATIONS operation, char * str);
void ast_tree_free(ast_node_t * head);

void ast_node_t_set_l_child(ast_node_t * parent, ast_node_t * l_child);
void ast_node_t_set_r_child(ast_node_t * parent, ast_node_t * r_child);

void ast_tree_print(ast_node_t * head);







#define MATH_PARSER_INPUT_BUFF_SIZE 501
#define PARSER_HISTORY_SIZE 30

typedef struct parser_hist_entry_t {
	char * input_str;
	int computed_result;
} parser_hist_entry_t;

typedef struct parser_history_t {
	parser_hist_entry_t hist_entries[PARSER_HISTORY_SIZE];

	int last_entry_num;

	int _size_filled;
	int _start_idx;

	char * _str_pool;
	int _str_pool_num_bytes;
} parser_history_t;

parser_history_t * parser_history_t_new();
void parser_history_t_free(parser_history_t * hist);

bool parser_history_t_push(parser_history_t * hist, char * input_str, int computed_result);
parser_hist_entry_t get_hist_entry_by_index(parser_history_t * hist, int index);
parser_hist_entry_t get_hist_entry_by_offset(parser_history_t * hist, int offset);
















void set_global_err(char * err_str);
bool is_global_err();
void print_global_err();
void reset_global_err();

int math_eval(mpz_t result, char * str);


#endif