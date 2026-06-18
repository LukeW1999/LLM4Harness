#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* nondet allocation for str (may be NULL) */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* bound the length of the string */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= max_len);

        /* allocate struct + flexible array space */
        str = malloc(sizeof(struct aws_string) + max_len - 1);
        __CPROVER_assume(str != NULL);

        /* allocator can be NULL (static) or a default allocator */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = str_len;
        /* bytes are part of the allocation; no need to initialise them */

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet allocation for cur (may be NULL) */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);

        /* ensure the cursor has an allocated buffer member */
        ensure_byte_cursor_has_allocated_buffer_member(cur);

        /* bound the cursor length */
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
        cur->len = cur_len;

        /* assume the cursor is bounded */
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    if (str) {
        old_str = *str;
    }
    struct aws_byte_cursor old_cur;
    if (cur) {
        old_cur = *cur;
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Assert postconditions for all cases */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        /* both non‑NULL: result must equal the array equality */
        assert(result == aws_array_eq(str->bytes, str->len, cur->ptr, cur->len));
    }

    /* 5. Assert unchanged fields (immutability) */
    if (str) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* bytes pointer (flexible array) must be unchanged */
        assert(str->bytes == old_str.bytes);
    }
    if (cur) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
    }

    /* 6. Assert validity invariants still hold */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (cur) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
