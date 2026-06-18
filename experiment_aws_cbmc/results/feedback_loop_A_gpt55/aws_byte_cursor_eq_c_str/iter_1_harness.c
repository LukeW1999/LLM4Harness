#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    struct aws_byte_cursor cursor;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len < SIZE_MAX);

    uint8_t c_str_storage[MAX_BUFFER_SIZE + 1];

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        c_str_storage[i] = nondet_uint8_t();
        if (i < c_str_len) {
            __CPROVER_assume(c_str_storage[i] != 0);
        }
    }
    c_str_storage[c_str_len] = 0;

    const char *c_str = (const char *)c_str_storage;
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));

    struct aws_byte_cursor old_cursor = cursor;

    uint8_t old_cursor_bytes[MAX_BUFFER_SIZE + 1];
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < cursor.len) {
            old_cursor_bytes[i] = cursor.ptr[i];
        }
    }

    uint8_t old_c_str_storage[MAX_BUFFER_SIZE + 1];
    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        old_c_str_storage[i] = c_str_storage[i];
    }

    bool expected = cursor.len == c_str_len;
    bool cursor_contains_null = false;

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < cursor.len) {
            if (cursor.ptr[i] == 0) {
                cursor_contains_null = true;
            }
            if (i < c_str_len && cursor.ptr[i] != c_str_storage[i]) {
                expected = false;
            }
        }
    }

    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    assert(result == expected);

    if (result) {
        assert(expected);
        assert(cursor.len == c_str_len);
        assert(!cursor_contains_null);

        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            if (i < cursor.len) {
                assert(cursor.ptr[i] == c_str_storage[i]);
            }
        }
    } else {
        assert(!expected);
    }

    if (cursor_contains_null) {
        assert(!result);
    }

    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < old_cursor.len) {
            assert(cursor.ptr[i] == old_cursor_bytes[i]);
        }
    }

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        assert(c_str_storage[i] == old_c_str_storage[i]);
    }

    assert(c_str_storage[c_str_len] == 0);

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < c_str_len) {
            assert(c_str_storage[i] != 0);
        }
    }

    assert(aws_byte_cursor_is_valid(&cursor));
}
