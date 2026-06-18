#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STRING_LEN 16
#define MAX_CSTR_LEN   16

void aws_string_eq_c_str_harness(void) {
    /* ---------- nondet allocation of aws_string ---------- */
    bool str_is_null = __CPROVER_nondet_bool();
    struct aws_string *str = NULL;
    size_t str_len = 0;
    uint8_t str_buf[MAX_STRING_LEN];

    if (!str_is_null) {
        str_len = __CPROVER_nondet_uint();
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        for (size_t i = 0; i < str_len; ++i) {
            str_buf[i] = __CPROVER_nondet_uint8();
        }

        str = aws_string_new_from_array(aws_default_allocator(), str_buf, str_len);
        __CPROVER_assume(str != NULL);
    }

    /* ---------- nondet allocation of C string ---------- */
    bool c_is_null = __CPROVER_nondet_bool();
    char *c_str = NULL;
    size_t c_len = 0;
    char c_buf[MAX_CSTR_LEN + 1];

    if (!c_is_null) {
        c_len = __CPROVER_nondet_uint();
        __CPROVER_assume(c_len <= MAX_CSTR_LEN);

        for (size_t i = 0; i < c_len; ++i) {
            c_buf[i] = __CPROVER_nondet_char();
            __CPROVER_assume(c_buf[i] != '\0');
        }
        c_buf[c_len] = '\0';
        c_str = c_buf;
    }

    /* ---------- precondition assumptions ---------- */
    __CPROVER_assume(! (str != NULL) || aws_string_is_valid(str));
    __CPROVER_assume(! (c_str != NULL) || aws_c_string_is_valid(c_str));

    /* ---------- call the function under verification ---------- */
    bool result = aws_string_eq_c_str(str, c_str);

    /* ---------- postcondition: return value correctness ---------- */
    if (str == NULL && c_str == NULL) {
        __CPROVER_assert(result == true,
                         "aws_string_eq_c_str returns true when both arguments are NULL");
    } else if (str == NULL || c_str == NULL) {
        __CPROVER_assert(result == false,
                         "aws_string_eq_c_str returns false when exactly one argument is NULL");
    } else {
        size_t c_actual_len = strlen(c_str);
        bool expected = (c_actual_len == str->len) &&
                        (memcmp(str->bytes, c_str, str->len) == 0);
        __CPROVER_assert(result == expected,
                         "aws_string_eq_c_str returns correct comparison result");
    }
}
