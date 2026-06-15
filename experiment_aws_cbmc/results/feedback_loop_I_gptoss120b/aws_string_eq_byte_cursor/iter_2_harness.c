#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    /* nondeterministic allocation of struct aws_string */
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
        /* bytes are part of the allocation; contents are nondet */
    }

    /* nondeterministic allocation of struct aws_byte_cursor */
    struct aws_byte_cursor *cur;
    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* save old state for frame condition checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur != NULL) {
        old_cur = *cur;
        save_byte_from_array(cur->ptr, cur->len, &old_cur_bytes);
    }

    /* call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* post‑condition checks */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
        assert(result == expected);
    }

    /* frame condition: unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_bytes);
    }

    /* validity predicates */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
