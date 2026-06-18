#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare possibly NULL inputs */
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* nondet decide whether to allocate a string */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate buffer on the stack large enough for any possible length */
        unsigned char str_buf[sizeof(struct aws_string) + MAX_BUFFER_SIZE - 1];
        str = (struct aws_string *)str_buf;

        /* set fields */
        str->len = len;
        str->allocator = NULL; /* static string */

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet decide whether to allocate a cursor */
    if (nondet_bool()) {
        struct aws_byte_cursor cur_obj;
        cur = &cur_obj;

        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;                     /* shallow copy of fields */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_buf;
    if (cur != NULL) {
        old_cur = *cur;                     /* shallow copy of fields */
        save_byte_from_array(cur->ptr, cur->len, &old_cur_buf);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Postconditions – function is pure, so everything unchanged */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_buf);
    }

    /* 5. Validity invariants must still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 6. No other side‑effects – result is simply the equality test */
    (void)result; /* silence unused‑variable warning */
}
