#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_harness_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

static void assert_string_unchanged(
    const struct aws_string *str,
    const struct aws_string *old_str,
    struct aws_allocator *old_allocator,
    size_t old_len,
    const uint8_t *old_bytes,
    const struct store_byte_from_buffer *old_byte) {

    assert(str == old_str);

    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert(str->bytes == old_bytes);
        assert_byte_from_buffer_matches(str->bytes, old_byte);
    }
}

static void assert_byte_buf_unchanged(
    const struct aws_byte_buf *buf,
    const struct aws_byte_buf *old_buf_ptr,
    const struct aws_byte_buf *old_buf,
    bool saved_old_buffer_byte,
    const struct store_byte_from_buffer *old_byte) {

    assert(buf == old_buf_ptr);

    if (buf != NULL) {
        assert(buf->allocator == old_buf->allocator);
        assert(buf->buffer == old_buf->buffer);
        assert(buf->len == old_buf->len);
        assert(buf->capacity == old_buf->capacity);

        if (saved_old_buffer_byte) {
            assert_byte_from_buffer_matches(buf->buffer, old_byte);
        }
    }
}

void aws_string_eq_byte_buf_harness() {
    struct aws_string_harness_storage *str_storage = NULL;
    const struct aws_string *str = NULL;

    if (nondet_bool()) {
        str_storage = malloc(sizeof(*str_storage));
        __CPROVER_assume(str_storage != NULL);

        str_storage->allocator = nondet_bool() ? NULL : aws_default_allocator();
        str_storage->len = nondet_size_t();
        __CPROVER_assume(str_storage->len <= MAX_BUFFER_SIZE);
        str_storage->bytes[str_storage->len] = 0;

        str = (const struct aws_string *)str_storage;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_byte_buf buf_storage;
    struct aws_byte_buf *buf = NULL;

    if (nondet_bool()) {
        buf = &buf_storage;
        buf_storage.allocator = aws_default_allocator();
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    const struct aws_string *old_str = str;
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

    const struct aws_byte_buf *old_buf_ptr = buf;
    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer old_buf_byte;
    bool saved_old_buf_byte = false;

    if (buf != NULL) {
        old_buf = *buf;
        if (buf->buffer != NULL && buf->capacity > 0) {
            save_byte_from_array(buf->buffer, buf->capacity, &old_buf_byte);
            saved_old_buf_byte = true;
        }
    }

    bool expected_result;
    if (str == NULL && buf == NULL) {
        expected_result = true;
    } else if (str == NULL || buf == NULL) {
        expected_result = false;
    } else {
        expected_result = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
    }

    bool result = aws_string_eq_byte_buf(str, buf);

    assert(result == expected_result);

    if (result) {
        assert(expected_result);
        assert_string_unchanged(str, old_str, old_str_allocator, old_str_len, old_str_bytes, &old_str_byte);
        assert_byte_buf_unchanged(buf, old_buf_ptr, &old_buf, saved_old_buf_byte, &old_buf_byte);
    } else {
        assert(!expected_result);
        assert_string_unchanged(str, old_str, old_str_allocator, old_str_len, old_str_bytes, &old_str_byte);
        assert_byte_buf_unchanged(buf, old_buf_ptr, &old_buf, saved_old_buf_byte, &old_buf_byte);
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
