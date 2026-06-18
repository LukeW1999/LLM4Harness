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

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string_harness_storage str_storage;
    str_storage.allocator = aws_default_allocator();
    str_storage.len = nondet_size_t();
    __CPROVER_assume(str_storage.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(str_storage.len < SIZE_MAX);

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        str_storage.bytes[i] = nondet_uint8_t();
    }
    str_storage.bytes[str_storage.len] = 0;

    const struct aws_string *nonnull_str = (const struct aws_string *)&str_storage;
    __CPROVER_assume(aws_string_is_valid(nonnull_str));

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    buf.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }
    assert(aws_byte_buf_is_valid(&buf));

    const struct aws_string *str = nondet_bool() ? NULL : nonnull_str;
    const struct aws_byte_buf *buf_ptr = nondet_bool() ? NULL : &buf;

    const struct aws_string *old_str_ptr = str;
    const struct aws_byte_buf *old_buf_ptr = buf_ptr;

    struct aws_allocator *old_str_allocator = str != NULL ? str->allocator : NULL;
    size_t old_str_len = str != NULL ? str->len : 0;

    struct aws_byte_buf old_buf = buf;

    struct store_byte_from_buffer old_str_byte;
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len + 1, &old_str_byte);
    }

    struct store_byte_from_buffer old_buf_byte;
    if (buf_ptr != NULL && buf_ptr->capacity > 0) {
        save_byte_from_array(buf_ptr->buffer, buf_ptr->capacity, &old_buf_byte);
    }

    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    assert(str == old_str_ptr);
    assert(buf_ptr == old_buf_ptr);

    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    }

    if ((str == NULL) != (buf_ptr == NULL)) {
        assert(result == false);
    }

    if (str != NULL && buf_ptr != NULL) {
        bool expected = aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len);
        assert(result == expected);

        if (result) {
            assert(str->len == buf_ptr->len);
            assert_bytes_match(str->bytes, buf_ptr->buffer, str->len);
        } else {
            assert(!aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len));
        }
    }

    if (result) {
        assert((str == NULL && buf_ptr == NULL) || (str != NULL && buf_ptr != NULL));
        if (str != NULL && buf_ptr != NULL) {
            assert(str->len == buf_ptr->len);
            assert_bytes_match(str->bytes, buf_ptr->buffer, str->len);
        }
    } else {
        assert(!(str == NULL && buf_ptr == NULL));
    }

    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_byte);
        assert(aws_string_is_valid(str));
    }

    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        if (old_buf.capacity > 0) {
            assert_byte_from_buffer_matches(buf_ptr->buffer, &old_buf_byte);
        }
        assert(aws_byte_buf_is_valid(buf_ptr));
    }
}
