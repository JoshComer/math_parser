#include "../src/syntax_parser.h"
#include "jc_test.h"


JC_TEST_FUNC syntax_number_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    lexer_token_list_t * token_list = lexer_token_list_t_tokenize_string("8");
    TEST_TRUE(syntax_number(NULL, token_list))





    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}









int main(int argc, char * argv[])
{
    JC_TEST_SET_TESTS_NAME("syntax_parser_tests")

    CHECK_TESTS(syntax_number_tests())




    JC_TEST_PRINT_SUCCESS()
    return 0;
}