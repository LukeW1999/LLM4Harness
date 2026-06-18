#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare inputs */
    struct aws_string *str;
    struct aws_byte_cursor cur;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a nondeterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        /* allocate enough memory for the flexible array member */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len - 1);
        __CPROVER_assume(str != NULL);
        /* allocator may be NULL (static string) or a valid allocator */
        str->allocator = NULL; /* static strings have NULL allocator */
        str->len = len;
        /* ensure the bytes buffer is allocated and bounded */
        if (len > 0) {
            __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, len));
        }
    }

    /* Ensure cursor has allocated buffer member and is bounded */
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    /* 2. Assume inputs are valid */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /* 3. Save old state for immutability checks */
    struct aws_string *old_str = str;
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL && str->len > 0) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur = cur;
    struct store_byte_from_buffer old_cur_bytes;
    if (cur.ptr != NULL && cur.len > 0) {
        save_byte_from_array(cur.ptr, cur.len, &old_cur_bytes);
    }

    /* 4. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* 5. Compute expected result using the specification */
    bool expected;
    if (str == NULL && cur.ptr == NULL) {
        expected = true;
    } else if (str == NULL || cur.ptr == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, cur.ptr, cur.len);
    }

    /* 6. Assert postconditions */
    assert(result == expected);

    /* Unchanged fields for str */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        if (str->len > 0) {
            assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
        }
    }

    /* Unchanged fields for cur */
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    if (cur.ptr != NULL && cur.len > 0) {
        assert_byte_from_buffer_matches(cur.ptr, &old_cur_bytes);
    }

    /* 7. Validity invariants must still hold */
    assert(str == NULL || aws_string_is_valid(str));
    assert(aws_byte_cursor_is_valid(&cur));
}
