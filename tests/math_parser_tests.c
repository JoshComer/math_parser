#include "../src/math_parser.h"
#include "jc_test.h"


JC_TEST_FUNC math_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    // lexer_token_list_t * list = _parse_tokenize("3 - 2 - 1");
    // printf("Lexer tokens\n");
    // for (int i = 0; i < list->size; i++)
    // {
    //     printf("%d:%s\n", i, list->tokens[i]->token_str);
    // }

    // printf("\nAST\n");
    // ast_node_t * tree_head = _parse_tokens_to_ast_tree(list);
    // ast_tree_print(tree_head);
    // printf("%d\n", _math_eval_recurse(tree_head));

    TEST_TRUE(math_eval("3 - 2 - 1") == 0)
    TEST_TRUE(math_eval("3 * 2 * 1") == 6)
    TEST_TRUE(math_eval("3 * 2 - 4") == 2)
    TEST_TRUE(math_eval("3 - 2 * 4") == -5)
    TEST_TRUE(math_eval("10 / 2 / 5") == 1)
    TEST_TRUE(math_eval("2 * (3 - 1)") == 4)
    

    TEST_TRUE(math_eval("2 - 5 * 7 - (3 - 2 - 1) + 2") == -31)
    TEST_TRUE(math_eval("5 + 3 * (4 - (3 - 2 - 1))") == 17)

    TEST_TRUE(math_eval("2^(2^(2^2))") == 65536)
    TEST_TRUE(math_eval("((1 - ((((((((1 + ((((((((((2 + 3))))))))))))))))))))") == -5)
    TEST_TRUE(math_eval("1 * 1 * 1 * 1 * 1 * (0 - 1) * 1 * 1 * 1") == -1)

    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}




int main(int argc, char * argv[])
{
    JC_TEST_SET_TESTS_NAME("math_parser_tests")

    CHECK_TESTS(math_tests())
    
    JC_TEST_PRINT_SUCCESS()

    return 0;
}