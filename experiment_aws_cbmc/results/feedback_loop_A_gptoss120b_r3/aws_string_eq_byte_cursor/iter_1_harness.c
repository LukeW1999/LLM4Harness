#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare and nondeterministically initialize inputs */
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* possibly NULL string */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate struct + flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);               /* allocation must succeed for the harness */
        str->allocator = aws_default_allocator();
        str->len = len;

        /* nondet fill the bytes */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* possibly NULL cursor */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        ensure_byte_cursor_has_allocated_buffer_member(cur);

        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);
        cur->len = len;
        cur->ptr = malloc(len);
        __CPROVER_assume(cur->ptr != NULL);

        /* nondet fill the cursor buffer */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)cur->ptr)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;                     /* copy header fields */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur != NULL) {
        old_cur = *cur;                     /* copy header fields */
        save_byte_from_array(cur->ptr, cur->len, &old_cur_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Assert postconditions */

    /* Relationship between inputs and result */
    if (str == NULL && cur == NULL) {
        assert(result);
    } else if (str == NULL || cur == NULL) {
        assert(!result);
    } else {
        assert(result == aws_array_eq(str->bytes, str->len, cur->ptr, cur->len));
    }

    /* Unchanged fields for str */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    /* Unchanged fields for cur */
    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_bytes);
    }

    /* 5. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
