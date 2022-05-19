
#include "history.h"
#include "iof_num.h"
#include "string.h"
#include "stdio.h"

parser_history_t * parser_history_t_new()
{
    parser_history_t * hist = malloc(sizeof(parser_history_t));
    if (hist == NULL)
        return NULL;

    // initialize each hist_entry and point each entry's string to its spot in the _str_pool
    for (int i = 0; i < PARSER_HISTORY_SIZE; i++)
    {
        hist->hist_entries[i].input_str = NULL;
        hist->hist_entries[i].input_str_len = -1;
        iof_init_int(&hist->hist_entries[i].computed_result); // init our result spot during construction
    }

    hist->_start_idx = 0;
    hist->_size_filled = 0;
    hist->last_entry_idx = 0;

    return hist;
}

void parser_history_t_free(parser_history_t * hist)
{
    if (hist != NULL)
        free(hist);
}

bool parser_history_t_push(parser_history_t * hist, char * input_str, iof_num * computed_result)
{
    if (hist == NULL)
        return false;
    
    // circle around to the first of the array if we've grown past the array size. If this happens we overwrite oldest entry
    int hist_entry_index = hist->last_entry_idx % PARSER_HISTORY_SIZE;
    parser_hist_entry_t * push_to = &(hist->hist_entries[hist_entry_index]);

    // Malloc space for string if there is no string mem yet, or there isn't enough space
    int input_str_len = strlen(input_str);
    if (input_str_len > push_to->input_str_len)
    {
        free(push_to->input_str); // will always work because C99 standard allows freeing NULL ptrs
        push_to->input_str = malloc(input_str_len * sizeof(char));
    }

    // copy data into hist_entry
    iof_copy_deep(&push_to->computed_result, computed_result);
    strcpy(push_to->input_str, input_str);

    hist->last_entry_idx++;
    
    // keep track of how many spots in our array have been used
    if (hist->_size_filled < PARSER_HISTORY_SIZE)
        hist->_size_filled++;

    // if we overwrote an entry set the start index to the spot past where we overwrote (the oldest entry)
    if (hist->last_entry_idx > PARSER_HISTORY_SIZE)
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
        parser_hist_entry_t empty = { .computed_result={0}, .input_str=NULL, .input_str_len=-1 };
        return empty;
    }

    if (offset > 0) // return most recent entry if offset is above range
        return hist->hist_entries[(hist->last_entry_idx - 1) % PARSER_HISTORY_SIZE];
    if (offset <= PARSER_HISTORY_SIZE * -1) // return oldest entry if offset is below range
        return hist->hist_entries[hist->_start_idx];

    return hist->hist_entries[(hist->last_entry_idx + offset - 1) % PARSER_HISTORY_SIZE];
}

parser_hist_entry_t get_hist_entry_by_index(parser_history_t * hist, int hist_num)
{
    if (hist == NULL || hist->last_entry_idx <= 0)
    {
        static parser_hist_entry_t empty;
        empty.input_str[0] = '\0';
        return empty;
    }

    // return latest history item if number is too big
    if (hist_num > hist->last_entry_idx)
        return hist->hist_entries[hist->last_entry_idx % PARSER_HISTORY_SIZE];
    if (hist_num < hist->last_entry_idx - PARSER_HISTORY_SIZE)
        return hist->hist_entries[hist->_start_idx];
    
    return hist->hist_entries[(hist_num - 1) % PARSER_HISTORY_SIZE];
}


void print_hist(parser_history_t * hist)
{
    // idx: result = input string
    // 1: 2 = 1 + 1
    // 2: 7 = variable 7
    // 3: 15 = variable + 8

    for(int i = 0; i < hist->_size_filled; i++)
    {
        parser_hist_entry_t entry = hist->hist_entries[(hist->_start_idx + i) % PARSER_HISTORY_SIZE];
        printf("%d: ", hist->last_entry_idx - hist->_size_filled + i + 1);
        iof_out_str(&entry.computed_result);
        printf(" = %s\n", entry.input_str);
    }
} 