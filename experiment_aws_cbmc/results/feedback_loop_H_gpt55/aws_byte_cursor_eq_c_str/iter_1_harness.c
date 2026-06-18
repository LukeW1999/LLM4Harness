#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness() {
    struct aws_byte_cursor cursor;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    char c_str[MAX_BUFFER_SIZE + 1];
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < c_str_len) {
            __CPROVER_assume(c_str[i] != '\0');
        }
    }
    c_str[c_str_len] = '\0';

    struct aws_byte_cursor old_cursor = cursor;

    struct store_byte_from_buffer cursor_storage;
    if (old_cursor.len > 0) {
        save_byte_from_array(old_cursor.ptr, old_cursor.len, &cursor_storage);
    }

    struct store_byte_from_buffer c_str_storage;
    save_byte_from_array((const uint8_t *)c_str, c_str_len + 1, &c_str_storage);

    bool expected = old_cursor.len == c_str_len;
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < old_cursor.len && i < c_str_len) {
            if (old_cursor.ptr[i] != (uint8_t)c_str[i]) {
                expected = false;
            }
        }
    }

    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    if (result) {
        assert(expected);
        assert(old_cursor.len == c_str_len);
        if (old_cursor.len > 0) {
            assert_bytes_match(old_cursor.ptr, (const uint8_t *)c_str, old_cursor.len);
        }
    } else {
        assert(!expected);
    }

    assert(result == expected);

    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    if (old_cursor.len > 0) {
        assert_byte_from_buffer_matches(cursor.ptr, &cursor_storage);
    }
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &c_str_storage);

    assert(aws_byte_cursor_is_valid(&cursor));
}
