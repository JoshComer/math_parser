#ifndef JC_TEST_HEADER
#define JC_TEST_HEADER

#include <string.h>

#define JC_TEST_ERROR_TEST_STR_SIZE 1024
#define JC_TEST_ERROR_EXPECTED_EVAL_STR_SIZE 100
typedef struct jc_test_error{
    bool is_error;
    int  test_num;
    char test_str[JC_TEST_ERROR_TEST_STR_SIZE];

    int expr_eval;
    char expected_eval[JC_TEST_ERROR_EXPECTED_EVAL_STR_SIZE];
} jc_test_error;

#define JC_TEST_FUNC jc_test_error
#define JC_TEST_FUNC_CONSTRUCT() jc_test_error _jc_test_error_magnum_6348 = { .is_error = false, .test_num = 1, .test_str = {'\0'}, .expr_eval = -434343, .expected_eval = {'\0'} };
#define JC_TEST_FUNC_DESTRUCT_AND_RETURN() return _jc_test_error_magnum_6348;

#define JC_TEST_SET_TESTS_NAME(string) char const * const _jc_tests_name_magnum_6348 = string;
#define JC_TEST_PRINT_SUCCESS() printf("All tests passed for %s\n", _jc_tests_name_magnum_6348);

 // strncat is used for safe copy of string if it's bigger than the allocated buffer
#define TEST_TRUE(argument)\
    { \
        int eval_to_int = (argument); \
        if ( ! eval_to_int ){\
            _jc_test_error_magnum_6348.is_error = true; \
            strncat(_jc_test_error_magnum_6348.test_str, #argument, JC_TEST_ERROR_TEST_STR_SIZE - 1);\
            _jc_test_error_magnum_6348.expr_eval = eval_to_int; \
            strncat(_jc_test_error_magnum_6348.expected_eval, "Non-Zero", JC_TEST_ERROR_EXPECTED_EVAL_STR_SIZE - 1);\
            return _jc_test_error_magnum_6348; \
        } \
        else{\
            _jc_test_error_magnum_6348.test_num++;\
        } \
    }

#define TEST_FALSE(argument)\
    { \
        int eval_to_int = (argument); \
        if ( eval_to_int ){\
            _jc_test_error_magnum_6348.is_error = true; \
            strncat(_jc_test_error_magnum_6348.test_str, #argument, JC_TEST_ERROR_TEST_STR_SIZE - 1);\
            _jc_test_error_magnum_6348.expr_eval = eval_to_int; \
            strncat(_jc_test_error_magnum_6348.expected_eval, "Zero", JC_TEST_ERROR_EXPECTED_EVAL_STR_SIZE - 1);\
            return _jc_test_error_magnum_6348; \
        } \
        else{\
            _jc_test_error_magnum_6348.test_num++;\
        } \
    }

#define TEST_ZERO(argument)\
    { \
        int eval_to_int = (argument); \
        if ( eval_to_int != 0 ){\
            _jc_test_error_magnum_6348.is_error = true; \
            strncat(_jc_test_error_magnum_6348.test_str, #argument, JC_TEST_ERROR_TEST_STR_SIZE - 1);\
            _jc_test_error_magnum_6348.expr_eval = eval_to_int; \
            strncat(_jc_test_error_magnum_6348.expected_eval, "Zero", JC_TEST_ERROR_EXPECTED_EVAL_STR_SIZE - 1);\
            return _jc_test_error_magnum_6348; \
        } \
        else{\
            _jc_test_error_magnum_6348.test_num++;\
        } \
    }

#define CHECK_TESTS(function) {jc_test_error ret_err = function; if (ret_err.is_error) { printf("-------------------------\ntest number %d failed for func %s\nResult was %d when expected was %s\nWas the following code, '%s'\n-------------------------\n", ret_err.test_num, #function, ret_err.expr_eval, ret_err.expected_eval, ret_err.test_str); return -1;}}

#endif