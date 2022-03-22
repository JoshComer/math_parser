#include "../src/math_parser.h"
#include "jc_test.h"


JC_TEST_FUNC math_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    TEST_TRUE(math_eval("1") == 1) // TODO: test -1 is -1
    TEST_TRUE(math_eval("0") == 0)
    TEST_TRUE(math_eval("1") == 1)

    TEST_TRUE(math_eval("3 - 2 - 1") == 0)
    TEST_TRUE(math_eval("3 * 2 * 1") == 6)
    TEST_TRUE(math_eval("3 * 2 - 4") == 2)
    TEST_TRUE(math_eval("3 - 2 * 4") == -5)
    TEST_TRUE(math_eval("10 / 2 / 5") == 1)
    TEST_TRUE(math_eval("2 * (3 - 1)") == 4)
    TEST_TRUE(math_eval("3 * 2 ^ 2") == 12)
    
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