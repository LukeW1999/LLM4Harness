#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        /* allocate struct with flexible array on the stack */
        struct aws_string *tmp = alloca(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        tmp->allocator = aws_default_allocator();
        tmp->len = len;
        /* bytes are nondeterministic (uninitialized memory) */
        __CPROVER_assume(aws_string_is_valid(tmp));
        str = tmp;
    }

    struct aws_byte_cursor cur_obj;
    struct aws_byte_cursor *cur;
    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = &cur_obj;
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
        /* allocate buffer for cursor on the stack */
        uint8_t *cbuf = alloca(cur_len);
        cur->ptr = cbuf;
        cur->len = cur_len;
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur) {
        old_cur = *cur;
        save_byte_from_array(cur->ptr, cur->len, &old_cur_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Compute expected result and assert correctness */
    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
    }
    assert(result == expected);

    /* 5. Assert unchanged fields (immutability) */
    if (str) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (cur) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_byte_from_buffer_matches(cur->ptr, &old_cur_bytes);
    }

    /* 6. Assert validity invariants after the call */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (cur) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
