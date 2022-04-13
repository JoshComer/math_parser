#ifndef JC_UTIL_HEADER
#define JC_UTIL_HEADER

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

// ------------------
// general char funcs
// ------------------

bool is_path_str(char * str, int str_len);
//bool is_path_existing(char * str, int str_len);
char * get_path_end(char * str);
bool char_in_list(const char compare, char const * const delimiters, const int num_delimiters);
char * get_home_str();
void print_bool_array(bool * arr, int size, bool newline);

char* itoa(int value, char* result, int base); // released under gpl3


// -------------
// string_list_t
// -------------

typedef struct string_list_t {
	char ** strings;
	int size;
	int _strings_allocated_spots;
} string_list_t, string_list_t_shallow;

string_list_t * string_list_t_new();
void string_list_t_free(string_list_t * list);

bool string_list_t_push_copy(string_list_t * list, char * string);
bool string_list_t_push_copy_from_idxs(string_list_t * list, char const * const source_str, int b_idx, int e_idx);
bool string_list_t_pop(string_list_t * list);
bool string_list_t_append_null(string_list_t * list);

bool string_list_t_contains(string_list_t * list, char * string);

void string_list_t_print_all(string_list_t * list);

string_list_t_shallow add_new_shallow_string(string_list_t_shallow list, char * string);



// ----------
// jc_stack_t
// ----------

#define JC_STACK_SIZE 1024
typedef struct jc_stack_t {
	char buffer[JC_STACK_SIZE];
	int size;
	int _allocated;
} jc_stack_t ;

jc_stack_t jc_stack_t_new();
bool	jc_stack_t_push(jc_stack_t * stack, char c);
bool	jc_stack_t_pop(jc_stack_t * stack);
char	jc_stack_t_peek(jc_stack_t * stack);
bool	jc_stack_t_reset(jc_stack_t * stack);
#define JC_STACK_T_IS_EMPTY(stack) (stack.size <= 0)

#endif