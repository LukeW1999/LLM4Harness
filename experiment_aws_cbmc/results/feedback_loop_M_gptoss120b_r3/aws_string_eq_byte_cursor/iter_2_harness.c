#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Nondeterministic inputs */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        struct aws_string str_local;
        str = &str_local;
        str->allocator = aws_default_allocator();
        str->len = len;
        ensure_aws_string_has_allocated_buffer_member(str);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_byte_cursor cur;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    /* 2. Save old state */
    struct aws_string *old_str_ptr = str;
    struct aws_string old_str_struct;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str_struct = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur = cur;
    struct store_byte_from_buffer old_cur_bytes;
    save_byte_from_array(cur.ptr, cur.len, &old_cur_bytes);

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* 4. Postconditions */

    /* Result semantics */
    if (str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cur.ptr, cur.len);
        assert(result == expected);
    }

    /* Unchanged fields for str */
    if (str != NULL) {
        assert(str->allocator == old_str_struct.allocator);
        assert(str->len == old_str_struct.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    } else {
        assert(old_str_ptr == NULL);
    }

    /* Unchanged fields for cur */
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    assert_byte_from_buffer_matches(cur.ptr, &old_cur_bytes);

    /* 5. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
