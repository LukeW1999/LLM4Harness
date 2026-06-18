#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_SIZE 256

void aws_string_eq_c_str_harness(void) {
    /* 1. Declare inputs */
    struct aws_string *str;
    const char *c_str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);

        /* stack‑allocated aws_string with flexible array space */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_SIZE];
        } str_wrapper;

        str = &str_wrapper.s;
        str->allocator = aws_default_allocator();
        str->len = len;

        /* nondet fill the bytes */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* ensure the string is considered valid */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* nondeterministically decide whether c_str is NULL */
    if (nondet_bool()) {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_SIZE);
        char *tmp = malloc(c_len + 1);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[c_len] = '\0';
        c_str = tmp;
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    } else {
        c_str = NULL;
    }

    /* 2. Save old state */
    struct aws_string old_str = {0};
    struct store_byte_from_buffer old_str_bytes = {0};
    if (str != NULL) {
        old_str.allocator = str->allocator;
        old_str.len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct store_byte_from_buffer old_c_str_bytes = {0};
    size_t c_str_len = 0;
    if (c_str != NULL) {
        while (c_str[c_str_len] != '\0') {
            ++c_str_len;
        }
        save_byte_from_array((const uint8_t *)c_str, c_str_len, &old_c_str_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Postconditions on return value */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    /* 5. Unchanged fields for str */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    /* 6. Unchanged bytes for c_str */
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_bytes);
    }

    /* 7. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
