#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../src/jc_util.h"

#include "jc_test.h"


JC_TEST_FUNC test_string_list_1()
{
    JC_TEST_FUNC_CONSTRUCT()

    string_list_t * list = string_list_t_new();
    TEST_TRUE(list != NULL)

    TEST_TRUE(string_list_t_push_copy(list, "testing"))
    TEST_TRUE(string_list_t_push_copy(list, ""))
    TEST_TRUE(string_list_t_push_copy(list, "testing"))
    TEST_TRUE(string_list_t_push_copy(list, "123"))
    /*  
    printf("should be 'testing'. Is:%s\nhas size of %d\n", list.strings[0], list.size);

    for (int i = 0; i < 4; i++)
    {
        printf("should be 'testing'. Is:%s\n", list.strings[i]);
    }

    printf("strcmp is %d\n", strcmp("testing", list.strings[0]));
    */  
    TEST_ZERO(strcmp("testing", list->strings[0]))
    TEST_ZERO(strcmp("", list->strings[1]))
    TEST_ZERO(strcmp("testing", list->strings[2]))
    TEST_ZERO(strcmp("123", list->strings[3]))

    TEST_TRUE(4 == list->size)

    string_list_t_free(list);
    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}

//JC_TEST_FUNC test_string_list_2()
//{
//    JC_TEST_FUNC_CONSTRUCT()
//
//        char const * const testing_str = "This is an extremely long string separated by spaces.\tThere's  also    some extra\nwhitespace\t\t added.";
//        const int tokens_sep_by_spaces = 15;
//        char const * const split_strs[15] = {"This", "is", "an", "extremely", "long", "string", "separated", "by", "spaces.", "There's", "also", "some", "extra", "whitespace", "added."};
//
//        string_list_t_shallow * list = //split_to_s_slice_t_on_delims(testing_str, strlen(testing_str), " \t\n", 3);
//        TEST_TRUE(list != NULL);
//        /*
//        printf("Size is %d\n", list.size);
//        for (int i = 0; i < list.size; i++)
//        {
//            printf("Token[%d]:%s\n", i, list.strings[i]);
//        }
//        */
//        TEST_TRUE(list->size == tokens_sep_by_spaces);
//        
//        for (int i = 0; i < list->size; i++)
//        {
//            printf("list\nsize:%d\nstrings%s\n", list->size, list->strings[0]);
//            TEST_ZERO(strcmp(list->strings[i], split_strs[i]))
//        }
//
//
//    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
//}

JC_TEST_FUNC jc_stack_t_tests()
{
    JC_TEST_FUNC_CONSTRUCT()

    jc_stack_t stack = jc_stack_t_new();

    TEST_TRUE(stack.size == 0);
    TEST_TRUE(jc_stack_t_peek(&stack) == '\0')
    TEST_FALSE(jc_stack_t_pop(&stack))

    char const * const test_str = "this is a testing string. It's not very interesting";

    for (int i = 0; i < (int)strlen(test_str); i++)
    {
        TEST_TRUE(jc_stack_t_push(&stack, test_str[i]))
        TEST_TRUE(jc_stack_t_peek(&stack) == test_str[i])

        // peek multiple times just in case that could mess things up
        for (int i = 0; i < 3; i++) jc_stack_t_peek(&stack);

        TEST_TRUE(stack.size == i + 1);
    }

    for (int i = (int)strlen(test_str) - 1; i > -1; i--)
    {
        TEST_TRUE(jc_stack_t_peek(&stack) == test_str[i])
        TEST_TRUE(jc_stack_t_pop(&stack))
    }

    TEST_TRUE(JC_STACK_T_IS_EMPTY(stack))

    JC_TEST_FUNC_DESTRUCT_AND_RETURN()
}


int main(int argc, char * argv[])
{
    JC_TEST_SET_TESTS_NAME("jc_util_tests")

    CHECK_TESTS(test_string_list_1())
    //CHECK_TESTS(test_string_list_2()) //
    CHECK_TESTS(jc_stack_t_tests())

    JC_TEST_PRINT_SUCCESS()

    return 0;
}