#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* Allocate storage for a string with up to MAX_BUFFER_SIZE bytes */
    union {
        struct aws_string s;
        uint8_t raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    } str_union;

    /* Allocate storage for a byte buffer */
    struct aws_byte_buf buf_storage;
    uint8_t buf_array[MAX_BUFFER_SIZE];

    if (nondet_bool()) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        str = &str_union.s;
        str->allocator = aws_default_allocator();
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (nondet_bool()) {
        buf = &buf_storage;
        buf->allocator = aws_default_allocator();
        size_t capacity = nondet_size_t();
        __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
        buf->buffer = buf_array;
        buf->capacity = capacity;
        buf->len = nondet_size_t();
        __CPROVER_assume(buf->len <= buf->capacity);
        for (size_t i = 0; i < buf->capacity; ++i) {
            buf->buffer[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }

    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes = {0};
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer old_buf_bytes = {0};
    if (buf != NULL) {
        old_buf = *buf;
        save_byte_from_array(buf->buffer, buf->capacity, &old_buf_bytes);
    }

    bool result = aws_string_eq_byte_buf(str, buf);

    if (str == NULL && buf == NULL) {
        __CPROVER_assert(result == true, "both NULL should be equal");
    } else if (str == NULL || buf == NULL) {
        __CPROVER_assert(result == false, "one NULL should be unequal");
    } else {
        bool expected = false;
        if (str->len == buf->len) {
            expected = (memcmp(str->bytes, buf->buffer, str->len) == 0);
        }
        __CPROVER_assert(result == expected, "result matches expected equality");
    }

    if (str != NULL) {
        __CPROVER_assert(str->allocator == old_str.allocator, "string allocator unchanged");
        __CPROVER_assert(str->len == old_str.len, "string length unchanged");
        assert_bytes_match(str->bytes, old_str_bytes.buffer, str->len);
    }

    if (buf != NULL) {
        __CPROVER_assert(buf->allocator == old_buf.allocator, "buffer allocator unchanged");
        __CPROVER_assert(buf->capacity == old_buf.capacity, "buffer capacity unchanged");
        __CPROVER_assert(buf->len == old_buf.len, "buffer length unchanged");
        __CPROVER_assert(buf->buffer == old_buf.buffer, "buffer pointer unchanged");
        assert_bytes_match(buf->buffer, old_buf_bytes.buffer, buf->capacity);
    }

    if (str != NULL) {
        __CPROVER_assert(aws_string_is_valid(str), "string remains valid");
    }
    if (buf != NULL) {
        __CPROVER_assert(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE), "buffer remains bounded");
    }
}
