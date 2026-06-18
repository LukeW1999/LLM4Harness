#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare and nondeterministically initialize inputs */
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = len;
        /* bytes are part of the allocation; they are nondeterministic */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_byte_cursor *cur;
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
    struct store_byte_from_buffer old_str_byte;
    if (str != NULL) {
        old_str = *str;
        if (str->len > 0) {
            save_byte_from_array(str->bytes, str->len, &old_str_byte);
        }
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_byte;
    if (cur != NULL) {
        old_cur = *cur;
        if (cur->len > 0) {
            save_byte_from_array(cur->ptr, cur->len, &old_cur_byte);
        }
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Assert postconditions for all cases */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
        assert(result == expected);
    }

    /* 5. Assert unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        if (str->len > 0) {
            assert_byte_from_buffer_matches(str->bytes, &old_str_byte);
        }
    }
    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        if (cur->len > 0) {
            assert_byte_from_buffer_matches(cur->ptr, &old_cur_byte);
        }
    }

    /* 6. Assert validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
