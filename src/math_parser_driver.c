#include "math_parser.h"


void replace_dollars_with_hist_vals(char * str)
{
    //bool dollar_flag = false;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '$') {}
            //dollar_flag = true;
    }
}


typedef enum PARSER_ACTION
{
    HIST,
    EVAL
} PARSER_ACTION;


PARSER_ACTION get_parser_action(char * input)
{
    if (strcmp(input, "hist\n") == 0)
        return HIST;
    else
        return EVAL;
}


int main(int argc, char * argv[])
{
    char buffer[MATH_PARSER_INPUT_BUFF_SIZE];
    parser_history_t * hist = parser_history_t_new();

    while (fgets(buffer, MATH_PARSER_INPUT_BUFF_SIZE, stdin))
    {
        PARSER_ACTION action = get_parser_action(buffer);

        if (action == HIST)
        {
            for(int i = 0; i < hist->_size_filled; i++)
            {
                parser_hist_entry_t entry = hist->hist_entries[(hist->_start_idx + i) % PARSER_HISTORY_SIZE];
                printf("%d: %d = %s", hist->last_entry_num - hist->_size_filled + i + 1, entry.computed_result, entry.input_str);
            }
        }
        else if (action == EVAL)
        {
            int result = math_eval(buffer);
            parser_history_t_push(hist, buffer, result);
            printf("%d\n", math_eval(buffer));
        }
        else
        {
            printf("Error: Uncoded action encountered\n");
            break;
        }
    }

    parser_history_t_free(hist);
    return 0;
}