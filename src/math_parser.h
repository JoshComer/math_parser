#ifndef JOSH_PARSER_HEADER
#define JOSH_PARSER_HEADER

#include "jc_util.h"

typedef enum LEXER_TOKEN_T_TYPES {
	_LEXER_TOKEN_T_TYPE_LOWER_BOUND = -1,
	LEXER_TOKEN_T_STRING_LITERAL = 0,
	LEXER_TOKEN_T_SPECIAL_CHAR = 1,
	LEXER_TOKEN_T_TEXT_TOKEN = 2,
	LEXER_TOKEN_T_INVALID_TOKEN = 3,

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
	bool _must_free;
} lexer_token_t;

lexer_token_t * lexer_token_t_new_legacy(char * str, int str_len);
lexer_token_t * lexer_token_t_new_empty();
lexer_token_t * lexer_token_t_new(LEXER_TOKEN_T_TYPES type, char * str);

bool lexer_token_t_move(lexer_token_t * dest, lexer_token_t * src);
//lexer_token_t lexer_token_t_new_stack(char * str, int str_len);
void lexer_token_t_free(lexer_token_t * token);


#define _JOSH_TOKEN_LIST_T_SIZE 1024
typedef struct lexer_token_list_t {
	lexer_token_t * tokens[_JOSH_TOKEN_LIST_T_SIZE];

	int size;
	int _allocated;

    int index;
} lexer_token_list_t;

lexer_token_list_t * lexer_token_list_t_new();
void lexer_token_list_t_free(lexer_token_list_t * list);

bool lexer_token_list_t_push_copy(lexer_token_list_t * list, LEXER_TOKEN_T_TYPES type, char * string);
bool lexer_token_list_t_push_and_move_token(lexer_token_list_t * list, lexer_token_t * token);
lexer_token_t * lexer_token_list_t_pop(lexer_token_list_t * list);
void lexer_token_t_set_type(lexer_token_t * token, LEXER_TOKEN_T_TYPES type);

void lexer_token_list_t_print_all(lexer_token_list_t * list);


typedef enum AST_NODE_T_TYPE {
	AST_NODE_T_TYPE_INVALID_TYPE = -1,
	AST_NODE_T_TYPE_TEXT,
	AST_NODE_T_TYPE_STRING_LITERAL,
	AST_NODE_T_TYPE_KEYWORD,


	AST_NODE_T_TYPE_MATOP,
    AST_NODE_T_TYPE_NUMBER,


	AST_NODE_T_TYPES_NUM_TYPES
} AST_NODE_T_TYPE;

typedef enum JOSH_OPERATIONS {
	JOSH_INVALID_OPER,

	JOSH_OPER_WHILE,
	JOSH_OPER_FOR,

	JOSH_OPER_IF,

	JOSH_OPER_OR,
	JOSH_OPER_AND,
	JOSH_OPER_NOT,

	JOSH_OPER_EQ,
	JOSH_OPER_GT,
	JOSH_OPER_LT,

	JOSH_OPER_MUL,
	JOSH_OPER_SUB,
	JOSH_OPER_ADD,
	JOSH_OPER_DIV,
	JOSH_OPER_MOD,
    JOSH_OPER_EXP

} JOSH_OPERATIONS;

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
	JOSH_OPERATIONS operation;
    AST_PRECEDENCE precedence;
} ast_node_t;

ast_node_t * ast_node_t_new_empty();
ast_node_t * ast_node_t_new(AST_NODE_T_TYPE type, JOSH_OPERATIONS operation, char * str);
void ast_tree_free(ast_node_t * head);

ast_node_t * lexer_tokens_to_ast_nodes(lexer_token_list_t * list);

void ast_tree_print(ast_node_t * head);







int math_eval(char * str);





#endif