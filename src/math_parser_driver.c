#include <stdio.h>
#include <gmp.h>

#include "iof_num.h"
#include "math_parser.h"
#include "history.h"


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
    NEWLINE,
    HIST,
    PRINT_VARS,
    EVAL
} PARSER_ACTION;


PARSER_ACTION get_parser_action(char * input)
{
    if (strlen(input) == 1)
        return NEWLINE;
    else if (strcmp(input, "hist\n") == 0)
        return HIST;
    else if (strcmp(input, "var_print\n") == 0)
        return PRINT_VARS;
    else
        return EVAL;
}

// TODO: Add tests for variables
int main(int argc, char * argv[])
{
    char buffer[MATH_PARSER_INPUT_BUFF_SIZE];
    parser_history_t * hist = parser_history_t_new();

    while (fgets(buffer, MATH_PARSER_INPUT_BUFF_SIZE, stdin))
    {
        PARSER_ACTION action = get_parser_action(buffer);

        if (action == NEWLINE)
        {
            // do nothing
        }
        else if (action == HIST)
        {
            print_hist(hist);
        }
        else if (action == PRINT_VARS)
        {
            print_label_table(get_interpreter_label_table());
        }
        else if (action == EVAL)
        {
            iof_num computed_result;
            iof_init_int(&computed_result);

            int eval_err = math_eval(&computed_result, buffer);

            if (is_global_err())
            {
                print_global_err();
                reset_global_err();
            }
            else
            {
                parser_history_t_push(hist, buffer, &computed_result);
                iof_out_str(&computed_result);
                //mpz_out_str(stdout, 10, computed_result.num.integer);
                putchar('\n');
            }
        }
        else
        {
            printf("Error: Uncoded action encountered\n");
            break;
        }

        // Blank line between output and next line
        putchar('\n');
    }

    parser_history_t_free(hist);
    return 0;
}