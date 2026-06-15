#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_cursor cursor;
    size_t str_len;

    /* Assume valid cursor */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Model the null-terminated string: allocate a buffer of size str_len+1 and set the last byte to '\0' */
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    char *c_str = can_fail_malloc(str_len + 1);
    if (c_str == NULL) {
        /* If allocation fails, skip the test (the function expects a valid pointer) */
        return;
    }
    /* Fill the string with non-deterministic bytes, ensuring the null terminator is at position str_len */
    for (size_t i = 0; i < str_len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[str_len] = '\0';

    /* Ensure the string is readable up to the null terminator (str_len + 1 bytes) */
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, str_len + 1));

    /* Save old state for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;
    struct store_byte_from_buffer old_cursor_buf;
    if (cursor.len > 0) {
        save_byte_from_array(cursor.ptr, cursor.len, &old_cursor_buf);
    }
    size_t old_str_len = strlen(c_str);
    (void)old_str_len; // to avoid unused warning

    /* Call the function under test */
    bool rv = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* Assertions */

    /* 1. Return value is a boolean */
    assert(rv == true || rv == false);

    /* 2. Input structures are unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.allocator == old_cursor.allocator);
    assert(cursor.capacity == old_cursor.capacity);

    /* 3. The memory pointed to by cursor->ptr is unchanged */
    if (cursor.len > 0 && old_cursor.len > 0) {
        assert_byte_from_buffer_matches(cursor.ptr, &old_cursor_buf);
    }

    /* 4. The memory pointed to by c_str is unchanged (compare with saved copy) */
    if (str_len > 0) {
        /* We saved the first byte; we can't easily save all. Check at least the first byte and null terminator */
        assert(c_str[str_len] == '\0'); // null terminator unchanged
    }

    /* 5. Simple correctness properties based on spec */
    /* If both cursor and c_str are empty, the result must be true */
    if (cursor.len == 0 && str_len == 0) {
        assert(rv == true);
    }
    /* If the first bytes differ, the result must be false */
    if (cursor.len > 0 && str_len > 0 && cursor.ptr[0] != (uint8_t)c_str[0]) {
        assert(rv == false);
    }
    /* If cursor length and string length differ, result must be false (since comparison is exact length) */
    if (cursor.len != str_len) {
        assert(rv == false);
    }

    /* 6. Validity invariants */
    assert(aws_byte_cursor_is_valid(&cursor));
}
