#ifndef MATH_PARSER_HISTORY_HEADER
#define MATH_PARSER_HISTORY_HEADER

#include "math_parser.h"
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





#endif