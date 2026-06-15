#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare nondeterministic inputs (may be NULL) */
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* nondeterministically decide if they are NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = len;
        /* bytes are left uninitialized – content is irrelevant for equality */
    }

    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    if (cur != NULL) {
        old_cur = *cur;
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Post‑condition on return value */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
