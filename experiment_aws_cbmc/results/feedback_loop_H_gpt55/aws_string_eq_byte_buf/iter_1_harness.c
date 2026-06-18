#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_harness_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

static bool s_byte_arrays_equal(const uint8_t *a, const uint8_t *b, size_t len) {
    bool equal = true;
    for (size_t i = 0; i < len; ++i) {
        if (a[i] != b[i]) {
            equal = false;
        }
    }
    return equal;
}

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string_harness_storage str_storage;
    str_storage.allocator = aws_default_allocator();
    str_storage.len = nondet_size_t();
    __CPROVER_assume(str_storage.len <= MAX_BUFFER_SIZE);
    str_storage.bytes[str_storage.len] = 0;

    const struct aws_string *valid_str = (const struct aws_string *)&str_storage;
    __CPROVER_assume(aws_string_is_valid(valid_str));

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    const struct aws_string *str = nondet_bool() ? valid_str : NULL;
    const struct aws_byte_buf *buf_ptr = nondet_bool() ? &buf : NULL;

    const struct aws_string *old_str_ptr = str;
    const struct aws_byte_buf *old_buf_ptr = buf_ptr;

    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    const uint8_t *old_str_bytes = NULL;
    struct store_byte_from_buffer old_str_byte;

    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        old_str_bytes = str->bytes;
        save_byte_from_array(str->bytes, str->len + 1, &old_str_byte);
    }

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_buf_byte;
    bool saved_buf_byte = false;
    if (buf_ptr != NULL && buf_ptr->capacity > 0) {
        save_byte_from_array(buf_ptr->buffer, buf_ptr->capacity, &old_buf_byte);
        saved_buf_byte = true;
    }

    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else if (str->len != buf_ptr->len) {
        expected = false;
    } else {
        expected = s_byte_arrays_equal(str->bytes, buf_ptr->buffer, str->len);
    }

    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    if (result) {
        assert(expected);
        assert(str == NULL || buf_ptr != NULL);
        assert(buf_ptr == NULL || str != NULL);
        if (str != NULL && buf_ptr != NULL) {
            assert(str->len == buf_ptr->len);
            assert_bytes_match(str->bytes, buf_ptr->buffer, str->len);
        }
    } else {
        assert(!expected);
    }

    assert(str == old_str_ptr);
    assert(buf_ptr == old_buf_ptr);

    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);
        assert_byte_from_buffer_matches(str->bytes, &old_str_byte);
        assert(aws_string_is_valid(str));
    }

    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        if (saved_buf_byte) {
            assert_byte_from_buffer_matches(buf_ptr->buffer, &old_buf_byte);
        }
        assert(aws_byte_buf_is_valid(buf_ptr));
    }

    assert(aws_string_is_valid(valid_str));
    assert(aws_byte_buf_is_valid(&buf));
}
