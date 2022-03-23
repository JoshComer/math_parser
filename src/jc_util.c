#include "jc_util.h"
#include <stdio.h>
#include <stdlib.h>


// ------------------
// general char funcs
// ------------------


bool is_path_str(char * str, int str_len)
{
	if (str_len <= 0)
		return false;
	else if (str[0] != '/')
		return false;

	return true; 
}

char * get_path_end(char * str)
{
	int size = strlen(str);

	if (size <= 0)
		return str;

	int e_idx = size;
	int b_idx = -1;

	for (int i = size - 1; i >= 0; i--)
	{
		if (str[i] == '/')
		{
			b_idx = i + 1;
			break;
		}
	}

	if (b_idx == -1)
		return str;

    //printf("string:%s\ne_idx:%d\nb_idx:%d\n", str, e_idx, b_idx);
	int ret_str_size = e_idx - b_idx;
	char * ret_str = malloc(ret_str_size);

	if (ret_str == NULL)
		return str;

	memcpy(ret_str, str + b_idx, ret_str_size);
    //printf("ret_str:%s\npassed_str:%s\n", ret_str, str);

	return ret_str;
}

bool char_in_list(const char compare, char const * const delimiters, const int num_delimiters)
{
	for (int i = 0; i < num_delimiters; i++)
	{
		if (delimiters[i] == compare)
			return true;
	}

	return false;
}

char * get_home_str()
{
    // TODO - check if I need to have an alternative if there is no HOME var set. https://stackoverflow.com/questions/2910377/get-home-directory-in-linux
	char * home_path = getenv("HOME");
	return home_path;
}

void print_bool_array(bool * arr, int size, bool newline)
{
	for (int i = 0; i < size; i++)
	{
		if (arr[i] == true)
			putchar('1');
		else
			putchar('0');
	}

	if (newline == true)
		putchar('\n');
}


	/**
	 * C++ version 0.4 char* style "itoa":
	 * Written by Lukás Chmela
	 * Released under GPLv3.

	 */
	char* itoa(int value, char* result, int base) {
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }

		char* ptr = result, *ptr1 = result, tmp_char;
		int tmp_value;

		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
		} while ( value );

		// Apply negative sign
		if (tmp_value < 0) *ptr++ = '-';
		*ptr-- = '\0';
		while(ptr1 < ptr) {
			tmp_char = *ptr;
			*ptr--= *ptr1;
			*ptr1++ = tmp_char;
		}
		return result;
	}





// -------------
// string_list_t
// -------------


string_list_t * string_list_t_new()
{
	const int initial_size = 1024;

	string_list_t * ret_list = malloc(sizeof(string_list_t));
	if (ret_list == NULL)
		return NULL;

	ret_list->strings = malloc(initial_size * sizeof(char *));

	if (ret_list->strings == NULL)
	{
		free(ret_list);
		return NULL;
	}


	ret_list->size = 0;
	ret_list->_strings_allocated_spots = initial_size;
	return ret_list;
}

void string_list_t_free(string_list_t * list)
{
	if (list == NULL)
		return;

	for (int i = 0; i < list->size; i++)
		free(list->strings[i]);

	free(list->strings);
}

string_list_t_shallow add_new_shallow_string(string_list_t_shallow list, char * string)
{
	if (list.size >= list._strings_allocated_spots)
		return list;

	list.strings[list.size] = string;
	list.size++;

	return list;
}

bool string_list_t_push_copy(string_list_t * list, char * string)
{
	if (string == NULL || list == NULL || list->size >= list->_strings_allocated_spots)
		return false;

	int string_size = strlen(string);

	list->strings[list->size] = malloc(string_size + 1);
	
	if (list->strings[list->size] == NULL)
		return false;
	
	strcpy(list->strings[list->size], string);

	list->size++;

	return true;
}

// copies string over from source AND appends null char
// TODO test function and get it working if needed
bool string_list_t_push_copy_from_idxs(string_list_t * list, char const * const source_str, int b_idx, int e_idx)
{
	if (list->size >= list->_strings_allocated_spots)
		return false;

	int string_size = e_idx - b_idx;

	list->strings[list->size] = malloc(string_size + 1);
	
	if (list->strings[list->size] == NULL)
		return false;
	
	memcpy(list->strings[list->size], source_str + b_idx, string_size);

	list->size++;

	return true;
}

bool string_list_t_pop(string_list_t *list)
{
	if (list == NULL || list->size <= 0)
		return false;

	free(list->strings[list->size - 1]);
	list->size--;

	return true;
}

bool string_list_t_append_null(string_list_t * list)
{
	if (list->size >= list->_strings_allocated_spots)
		return false;

	list->strings[list->size] = (char *)NULL;
	list->size++;

	return true;
}

void string_list_t_print_all(string_list_t * list)
{
	printf("Printing String List\n");
	for (int i = 0; i < list->size; i++)
	{
		printf("%d:%s\n", i, list->strings[i]);
	}
}






// ----------
// jc_stack_t
// ----------

jc_stack_t jc_stack_t_new()
{
	jc_stack_t ret_stack;
	ret_stack._allocated = JC_STACK_SIZE;
	ret_stack.size = 0;

	return ret_stack;
}

bool	jc_stack_t_push(jc_stack_t * stack, char c){
	if (stack == NULL || stack->size >= JC_STACK_SIZE - 1)
		return false;

	stack->buffer[stack->size] = c;
	stack->buffer[stack->size + 1] = '\0';
	stack->size++;
	return true;
}

bool	jc_stack_t_pop(jc_stack_t * stack){
	if (stack == NULL || stack->size <= 0)
		return false;

	stack->buffer[stack->size - 1] = '\0';
	stack->size--;
	return true;
}

bool	jc_stack_t_reset(jc_stack_t * stack)
{
	if (stack == NULL)
		return false;

	stack->size = 0;
	stack->buffer[0] = '\0';

	return true;
}

char	jc_stack_t_peek(jc_stack_t * stack){
	if (stack == NULL || stack->size <= 0)
		return '\0';

	return stack->buffer[stack->size - 1];
}
