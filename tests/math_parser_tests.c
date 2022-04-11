#include "../src/math_parser.h"
#include "jc_test.h"


JC_TEST_FUNC basic_history_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    parser_history_t * hist = parser_history_t_new();

    for(int i = 1; i < PARSER_HISTORY_SIZE + 1; i++)
    {
        char num_buf[10];
        parser_history_t_push(hist, itoa(i, num_buf, 10), i);
    }

    for(int i = 1; i < PARSER_HISTORY_SIZE + 1; i++)
    {
        TEST_TRUE(get_hist_entry_by_index(hist, i).computed_result == i)
    }

    parser_history_t_free(hist);

    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}


JC_TEST_FUNC more_history_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    parser_history_t * hist = parser_history_t_new();

    for(int i = 1; i < PARSER_HISTORY_SIZE + 6; i++)
    {
        char num_buf[10];
        parser_history_t_push(hist, itoa(i, num_buf, 10), i);
    }

    for (int i = 6, j = -PARSER_HISTORY_SIZE + 1; i < PARSER_HISTORY_SIZE + 6; i++, j++)
    {
        TEST_TRUE(get_hist_entry_by_index(hist, i).computed_result == i)
        TEST_TRUE(get_hist_entry_by_offset(hist, j).computed_result == i)
    }

    parser_history_t_free(hist);

    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}


JC_TEST_FUNC math_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    iof_num _computed_result;
    iof_num * computed_result = &_computed_result;
    iof_init_int(computed_result);
    TEST_ZERO(math_eval(computed_result, "1"))
    TEST_ZERO(iof_cmp_si(computed_result, 1)) // TODO: test -1 is -1
    TEST_ZERO(math_eval(computed_result, "0"))
    TEST_ZERO(iof_cmp_si(computed_result, 0))
    TEST_ZERO(math_eval(computed_result, "1"))
    TEST_ZERO(iof_cmp_si(computed_result, 1))

    TEST_ZERO(math_eval(computed_result, "3 - 2 - 1"))
    TEST_ZERO(iof_cmp_si(computed_result, 0))
    TEST_ZERO(math_eval(computed_result, "3 * 2 * 1"))
    TEST_ZERO(iof_cmp_si(computed_result, 6))
    TEST_ZERO(math_eval(computed_result, "3 * 2 - 4"))
    TEST_ZERO(iof_cmp_si(computed_result, 2))
    TEST_ZERO(math_eval(computed_result, "3 - 2 * 4"))
    TEST_ZERO(iof_cmp_si(computed_result, -5))
    TEST_ZERO(math_eval(computed_result, "10 / 2 / 5"))
    TEST_ZERO(iof_cmp_si(computed_result, 1))
    TEST_ZERO(math_eval(computed_result, "2 * (3 - 1)"))
    TEST_ZERO(iof_cmp_si(computed_result, 4))
    TEST_ZERO(math_eval(computed_result, "3 * 2 ^ 2"))
    TEST_ZERO(iof_cmp_si(computed_result, 12))
    
    TEST_ZERO(math_eval(computed_result, "2 - 5 * 7 - (3 - 2 - 1) + 2"))
    TEST_ZERO(iof_cmp_si(computed_result, -31))
    TEST_ZERO(math_eval(computed_result, "5 + 3 * (4 - (3 - 2 - 1))"))
    TEST_ZERO(iof_cmp_si(computed_result, 17))

    TEST_ZERO(math_eval(computed_result, "2^(2^(2^2))"))
    TEST_ZERO(iof_cmp_si(computed_result, 65536))
    TEST_ZERO(math_eval(computed_result, "((1 - ((((((((1 + ((((((((((2 + 3))))))))))))))))))))"))
    TEST_ZERO(iof_cmp_si(computed_result, -5))
    TEST_ZERO(math_eval(computed_result, "1 * 1 * 1 * 1 * 1 * (0 - 1) * 1 * 1 * 1"))
    TEST_ZERO(iof_cmp_si(computed_result, -1))

    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}


int main(int argc, char * argv[])
{
    JC_TEST_SET_TESTS_NAME("math_parser_tests")

    CHECK_TESTS(basic_history_tests())
    CHECK_TESTS(more_history_tests())

    CHECK_TESTS(math_tests())

    JC_TEST_PRINT_SUCCESS()

    return 0;
}