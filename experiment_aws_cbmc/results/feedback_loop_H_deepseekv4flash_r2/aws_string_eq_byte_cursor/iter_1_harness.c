#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_STRING_SIZE 100

void aws_string_eq_byte_cursor_harness() {
    /* nondeterministic choices for null pointers */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* nondet for equality of bytes (if both non-null) */
    bool arrays_equal = nondet_bool();

    /* size for string and cursor */
    size_t str_len = nondet_size_t();
    size_t cur_len = nondet_size_t();
    __CPROVER_assume(str_len < MAX_STRING_SIZE);
    __CPROVER_assume(cur_len < MAX_STRING_SIZE);

    /* allocate and initialize string if non-null */
    if (!str_is_null) {
        str = malloc(sizeof(struct aws_string) + str_len + 1); /* +1 for null terminator */
        __CPROVER_assume(str != NULL);
        /* set const fields (CBMC doesn't enforce const) */
        str->allocator = aws_default_allocator();
        str->len = str_len;
        /* fill bytes nondeterministically, including null terminator */
        for (size_t i = 0; i < str_len; i++) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[str_len] = '\0'; /* optional but good practice */
        /* ensure the bytes are readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str_len));
    }

    /* allocate and initialize cursor if non-null */
    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        cur->len = cur_len;
        cur->ptr = malloc(cur_len);
        __CPROVER_assume(cur->ptr != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(cur->ptr, cur_len));
        /* fill cursor bytes */
        for (size_t i = 0; i < cur_len; i++) {
            cur->ptr[i] = nondet_uint8_t();
        }
    }

    /* if both non-null and lengths equal, we might force equality or inequality */
    if (!str_is_null && !cur_is_null && str_len == cur_len) {
        if (arrays_equal) {
            /* force all bytes to match */
            for (size_t i = 0; i < str_len; i++) {
                __CPROVER_assume(str->bytes[i] == cur->ptr[i]);
            }
        } else {
            /* force at least one byte to differ */
            if (str_len > 0) {
                size_t diff_idx = nondet_size_t();
                __CPROVER_assume(diff_idx < str_len);
                __CPROVER_assume(str->bytes[diff_idx] != cur->ptr[diff_idx]);
            } else {
                /* lengths are 0, arrays are trivially equal; arrays_equal must be true */
                __CPROVER_assume(arrays_equal == true);
            }
        }
    }

    /* save old state for immutability checks */
    struct store_byte_from_buffer saved_str_bytes[MAX_STRING_SIZE];
    struct store_byte_from_buffer saved_cur_bytes[MAX_STRING_SIZE];
    if (!str_is_null) {
        save_byte_from_array(str->bytes, str_len, saved_str_bytes);
    }
    if (!cur_is_null) {
        save_byte_from_array(cur->ptr, cur_len, saved_cur_bytes);
    }

    /* call function */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* postconditions */
    if (str_is_null && cur_is_null) {
        assert(result == true);
    } else if (str_is_null || cur_is_null) {
        assert(result == false);
    } else {
        if (str_len == cur_len && arrays_equal) {
            assert(result == true);
        } else {
            assert(result == false);
        }
    }

    /* immutability: only readable data must not change */
    if (!str_is_null) {
        assert_byte_from_buffer_matches(str->bytes, saved_str_bytes);
    }
    if (!cur_is_null) {
        assert_byte_from_buffer_matches(cur->ptr, saved_cur_bytes);
    }

    /* validity: both inputs unchanged and still valid */
    if (!str_is_null) {
        assert(aws_string_is_valid(str));
    }
    if (!cur_is_null) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
