#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor cur;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        uint8_t raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)raw;
        str->allocator = aws_default_allocator(); /* non‑static string */
        str->len = len;                           /* set length */
        /* the flexible array member `bytes` is part of the allocation; contents are nondet */
    }

    __CPROVER_assume(aws_string_is_valid(str));

    /* prepare a byte cursor with allocated buffer and bounded size */
    uint8_t cur_buf[MAX_BUFFER_SIZE];
    cur.ptr = cur_buf;
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    /* Save old state for immutability checks */
    struct aws_string *old_str = NULL;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        uint8_t old_raw[sizeof(struct aws_string)];
        old_str = (struct aws_string *)old_raw;
        *old_str = *str;                     /* copy allocator and len */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur = cur;
    struct store_byte_from_buffer old_cur_bytes;
    save_byte_from_array(cur.ptr, cur.len, &old_cur_bytes);

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* Post‑condition assertions */
    if (str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cur.ptr, cur.len);
        assert(result == expected);
    }

    /* Unchanged fields – string */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    /* Unchanged fields – cursor */
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    assert_byte_from_buffer_matches(cur.ptr, &old_cur_bytes);

    /* Validity invariants after the call */
    assert(aws_string_is_valid(str));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
