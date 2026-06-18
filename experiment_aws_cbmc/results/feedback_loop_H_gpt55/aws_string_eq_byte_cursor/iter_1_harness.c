#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_bounded_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

void aws_string_eq_byte_cursor_harness() {
    struct aws_string_bounded_storage str_storage;
    str_storage.allocator = nondet_bool() ? NULL : aws_default_allocator();
    str_storage.len = nondet_size_t();
    __CPROVER_assume(str_storage.len <= MAX_BUFFER_SIZE);

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        str_storage.bytes[i] = nondet_uint8_t();
    }
    str_storage.bytes[str_storage.len] = 0;

    const struct aws_string *str =
        nondet_bool() ? NULL : (const struct aws_string *)&str_storage;
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    struct aws_byte_cursor cursor;
    cursor.len = nondet_size_t();
    __CPROVER_assume(cursor.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(cursor.len == 0 || AWS_MEM_IS_WRITABLE(cursor.ptr, cursor.len));

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < cursor.len) {
            cursor.ptr[i] = nondet_uint8_t();
        }
    }
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    const struct aws_byte_cursor *cur = nondet_bool() ? NULL : &cursor;

    struct aws_byte_cursor old_cursor = cursor;

    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    const uint8_t *old_str_bytes = NULL;
    struct store_byte_from_buffer old_str_byte;
    bool saved_str_byte = false;

    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        old_str_bytes = str->bytes;
        save_byte_from_array(str->bytes, str->len + 1, &old_str_byte);
        saved_str_byte = true;
    }

    struct store_byte_from_buffer old_cursor_byte;
    bool saved_cursor_byte = false;
    if (cursor.len > 0) {
        save_byte_from_array(cursor.ptr, cursor.len, &old_cursor_byte);
        saved_cursor_byte = true;
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        expected = str->len == cur->len;
        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            if (expected && i < str->len && str->bytes[i] != cur->ptr[i]) {
                expected = false;
            }
        }
    }

    assert(result == expected);

    if (result) {
        if (str == NULL || cur == NULL) {
            assert(str == NULL && cur == NULL);
        } else {
            assert(str->len == cur->len);
            for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
                if (i < str->len) {
                    assert(str->bytes[i] == cur->ptr[i]);
                }
            }
        }
    } else {
        assert(!(str == NULL && cur == NULL));
        if (str != NULL && cur != NULL) {
            if (str->len == cur->len) {
                bool mismatch_found = false;
                for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
                    if (i < str->len && str->bytes[i] != cur->ptr[i]) {
                        mismatch_found = true;
                    }
                }
                assert(mismatch_found);
            } else {
                assert(str->len != cur->len);
            }
        } else {
            assert(str == NULL || cur == NULL);
        }
    }

    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);
        if (saved_str_byte) {
            assert_byte_from_buffer_matches(str->bytes, &old_str_byte);
        }
    }

    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    if (saved_cursor_byte) {
        assert_byte_from_buffer_matches(cursor.ptr, &old_cursor_byte);
    }

    assert(str == NULL || aws_string_is_valid(str));
    assert(cur == NULL || aws_byte_cursor_is_valid(cur));
    assert(aws_byte_cursor_is_valid(&cursor));
}
