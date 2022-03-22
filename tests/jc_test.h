#ifndef JC_TEST_HEADER
#define JC_TEST_HEADER

#include <string.h>

#define JC_TEST_ERROR_TEST_STR_SIZE 1024
typedef struct jc_test_error{
    bool is_error;
    int  test_num;
    char test_str[JC_TEST_ERROR_TEST_STR_SIZE];
} jc_test_error;

#define JC_TEST_FUNC jc_test_error
#define JC_TEST_FUNC_CONSTRUCT() jc_test_error _jc_test_error_magnum_6348 = { .is_error = false, .test_num = 1, .test_str = {'\0'} };
#define JC_TEST_FUNC_DESTRUCT_AND_RETURN() return _jc_test_error_magnum_6348;

#define JC_TEST_SET_TESTS_NAME(string) char const * const _jc_tests_name_magnum_6348 = string;
#define JC_TEST_PRINT_SUCCESS() printf("All tests passed for %s\n", _jc_tests_name_magnum_6348);

 // strncat is used for safe copy of string if it's bigger than the alloted buffer
#define TEST_TRUE(argument)\
    if (! (argument) ){\
        _jc_test_error_magnum_6348.is_error = true; \
        strncat(_jc_test_error_magnum_6348.test_str, #argument, JC_TEST_ERROR_TEST_STR_SIZE - 1);\
        return _jc_test_error_magnum_6348; \
    } \
    else{\
        _jc_test_error_magnum_6348.test_num++;\
    }

#define TEST_FALSE(argument)\
    if ( (argument) ){\
        _jc_test_error_magnum_6348.is_error = true; \
        strncat(_jc_test_error_magnum_6348.test_str, #argument, JC_TEST_ERROR_TEST_STR_SIZE - 1);\
        return _jc_test_error_magnum_6348; \
    } \
    else{\
        _jc_test_error_magnum_6348.test_num++;\
    }

#define TEST_ZERO(argument)\
    if ( (argument) != 0 ){\
        _jc_test_error_magnum_6348.is_error = true; \
        strncat(_jc_test_error_magnum_6348.test_str, #argument, JC_TEST_ERROR_TEST_STR_SIZE - 1);\
        return _jc_test_error_magnum_6348; \
    } \
    else{\
        _jc_test_error_magnum_6348.test_num++;\
    }

#define CHECK_TESTS(function) {jc_test_error ret_err = function; if (ret_err.is_error) { printf("test number %d failed for func %s. Was the following code, '%s'\n", ret_err.test_num, #function, ret_err.test_str); return -1;}}

#endif