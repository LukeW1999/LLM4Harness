#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare inputs (may be NULL) */
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a nondeterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        /* allocate enough memory for the flexible array member */
        str = malloc(sizeof(struct aws_string) + (len == 0 ? 0 : len - 1));
        __CPROVER_assume(str != NULL);
        /* fields */
        str->allocator = NULL;               /* allocator not relevant for equality */
        str->len = len;
        /* bytes are already part of the allocation; they are readable */
    }

    /* nondeterministically decide if cur is NULL */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        /* ensure the cursor has an allocated buffer member */
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        /* bound the cursor */
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state */
    struct aws_string old_str_copy;
    if (str != NULL) {
        old_str_copy = *str;
    }
    struct aws_byte_cursor old_cur_copy;
    if (cur != NULL) {
        old_cur_copy = *cur;
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Postconditions */

    /* 4a. Result must match the specification */
    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
    }
    assert(result == expected);

    /* 4b. Unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str_copy.allocator);
        assert(str->len == old_str_copy.len);
        assert(str->bytes == old_str_copy.bytes);
    }
    if (cur != NULL) {
        assert(cur->ptr == old_cur_copy.ptr);
        assert(cur->len == old_cur_copy.len);
    }

    /* 4c. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 5. Clean up */
    free(str);
    free(cur);
}
