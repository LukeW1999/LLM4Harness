#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>

#include "proof_helpers/make_common_data_structures.h"

/* Harness for aws_string_eq_byte_cursor */
void aws_string_eq_byte_cursor_harness(void) {
    /* nondeterministically decide whether the pointers are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    /* -------------------------------------------------
     *  Set up aws_string *str
     * ------------------------------------------------- */
    struct aws_string *str = NULL;
    struct store_byte_from_buffer old_str_bytes;
    struct aws_string old_str;               /* shallow copy for scalar fields */

    if (!str_is_null) {
        /* bound the length of the string */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate enough memory for the struct + flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* initialise fields */
        str->allocator = NULL;               /* allocator is not relevant for this function */
        str->len = len;

        /* fill the byte array with nondet data */
        uint8_t *bytes = (uint8_t *)str->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* save old state for later comparison */
        old_str = *str;                       /* copy scalar fields */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    /* -------------------------------------------------
     *  Set up aws_byte_cursor *cur
     * ------------------------------------------------- */
    struct aws_byte_cursor *cur = NULL;
    struct store_byte_from_buffer old_cur_bytes;
    struct aws_byte_cursor old_cur;         /* shallow copy for scalar fields */

    if (!cur_is_null) {
        /* bound the length of the cursor */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate the cursor struct */
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);

        /* allocate the underlying buffer */
        uint8_t *buf = NULL;
        if (len > 0) {
            buf = malloc(len);
            __CPROVER_assume(buf != NULL);
        }

        /* fill the buffer with nondet data */
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }

        cur->ptr = buf;
        cur->len = len;

        /* assume the cursor is bounded */
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));

        /* save old state for later comparison */
        old_cur = *cur;                       /* copy scalar fields */
        save_byte_from_array(cur->ptr, cur->len, &old_cur_bytes);
    }

    /* -------------------------------------------------
     *  Call the function under test
     * ------------------------------------------------- */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* -------------------------------------------------
     *  Post‑condition: result must match the specification
     * ------------------------------------------------- */
    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
    }
    assert(result == expected);

    /* -------------------------------------------------
     *  Unchanged fields (immutability)
     * ------------------------------------------------- */
    if (str != NULL) {
        assert(str->len == old_str.len);
        assert(str->allocator == old_str.allocator);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_bytes);
    }

    /* -------------------------------------------------
     *  Validity invariants must still hold
     * ------------------------------------------------- */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
