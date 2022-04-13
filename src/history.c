
#include "history.h"

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
