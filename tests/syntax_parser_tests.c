#include "../src/syntax_parser.h"
#include "jc_test.h"


// these functions take care of memory allocation so we can directly test expressions in one line
int test_direct_check_syntactically_correct(char * string)
{
    lexer_token_list_t * token_list = lexer_token_list_t_tokenize_string(string);
    int ret_val = check_syntactically_correct(token_list);
    free(token_list);

    return ! ret_val; // return true if ret_val is 0. It'd be better if everything was cohesive in return value, but I'll get to it later
}


JC_TEST_FUNC syntax_number_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    TEST_TRUE(test_direct_check_syntactically_correct("8"))
    TEST_TRUE(test_direct_check_syntactically_correct("99999"))

    //TEST_FALSE(test_direct_check_syntactically_correct("+"))
    //TEST_FALSE(test_direct_check_syntactically_correct(")"))
    //TEST_FALSE(test_direct_check_syntactically_correct("("))
    //TEST_FALSE(test_direct_check_syntactically_correct("["))
    //TEST_FALSE(test_direct_check_syntactically_correct("]"))

    // TODO: Add more tests testing labels
    // TODO: Add more tests after adding functions

    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}


JC_TEST_FUNC syntax_matop_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    // testing for when the number is chained with something else
    TEST_TRUE(test_direct_check_syntactically_correct("8 + 8"))
    TEST_TRUE(test_direct_check_syntactically_correct("0 - 0"))
    TEST_TRUE(test_direct_check_syntactically_correct("3 / 0"))
    TEST_TRUE(test_direct_check_syntactically_correct("0 * 999999"))
    TEST_TRUE(test_direct_check_syntactically_correct("0000009 % 0"))
    
    TEST_FALSE(test_direct_check_syntactically_correct("0 - a"))
    TEST_FALSE(test_direct_check_syntactically_correct("7 8 9"))
    TEST_FALSE(test_direct_check_syntactically_correct("+ 1 2"))
    TEST_FALSE(test_direct_check_syntactically_correct("1 2 +"))

    // TODO: Add more tests testing labels
    // TODO: Add more tests after adding functions

    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}







JC_TEST_FUNC syntax_comprehensive_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    TEST_TRUE(test_direct_check_syntactically_correct("8 + 8 + 8"))
    TEST_TRUE(test_direct_check_syntactically_correct("3 - 234565 + 99999 * 0"))

    TEST_TRUE(test_direct_check_syntactically_correct("def int seven {9 - 2}"))
    TEST_TRUE(test_direct_check_syntactically_correct("def int seven {def int eight 8}"))

    TEST_TRUE(test_direct_check_syntactically_correct("(5 + (3 - (def int seven {7 + 2})))"))


    TEST_TRUE(test_direct_check_syntactically_correct("(def int func [] {8 + 3}) []"))
    TEST_FALSE(test_direct_check_syntactically_correct("(func [] {8 + 3}) []"))


    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}






int main(int argc, char * argv[])
{
    JC_TEST_SET_TESTS_NAME("syntax_parser_tests")

    CHECK_TESTS(syntax_number_tests())
    CHECK_TESTS(syntax_matop_tests())


    CHECK_TESTS(syntax_comprehensive_tests())


    JC_TEST_PRINT_SUCCESS()
    return 0;
}