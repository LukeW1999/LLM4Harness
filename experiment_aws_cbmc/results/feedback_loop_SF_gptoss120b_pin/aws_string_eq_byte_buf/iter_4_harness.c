#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic string */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;
    struct {
        struct aws_string s;
        uint8_t buf[1024];
    } str_obj;

    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= 1024);
        str = (struct aws_string *)&str_obj;
        str->allocator = allocator;
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    }

    /* nondeterministic byte buffer */
    bool buf_is_null = nondet_bool();
    struct aws_byte_buf *buf = NULL;
    struct aws_byte_buf buf_obj;
    uint8_t buf_storage[1024];

    if (!buf_is_null) {
        size_t buf_len = nondet_size_t();
        __CPROVER_assume(buf_len <= 1024);
        buf = &buf_obj;
        buf->allocator = allocator;
        buf->capacity = buf_len;
        buf->len = buf_len;
        buf->buffer = buf_storage;
        for (size_t i = 0; i < buf_len; ++i) {
            buf->buffer[i] = nondet_uint8_t();
        }
    }

    /* preconditions */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    __CPROVER_assume(buf == NULL || aws_byte_buf_is_valid(buf));

    /* snapshot of input state */
    size_t old_str_len = 0;
    const uint8_t *old_str_bytes = NULL;
    if (str) {
        old_str_len = str->len;
        old_str_bytes = str->bytes;
    }

    size_t old_buf_len = 0;
    size_t old_buf_capacity = 0;
    uint8_t *old_buf_buffer = NULL;
    if (buf) {
        old_buf_len = buf->len;
        old_buf_capacity = buf->capacity;
        old_buf_buffer = buf->buffer;
    }

    /* call under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* postconditions */
    if (str == NULL || buf == NULL) {
        assert(!result);
    } else {
        assert(str->len == old_str_len);
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(str->allocator == allocator);
        assert(buf->allocator == allocator);
        assert(str->bytes == old_str_bytes);
        assert(buf->buffer == old_buf_buffer);

        bool lengths_eq = (str->len == buf->len);
        bool bytes_eq = true;
        for (size_t i = 0; i < str->len; ++i) {
            if (((uint8_t *)str->bytes)[i] != buf->buffer[i]) {
                bytes_eq = false;
                break;
            }
        }
        assert(result == (lengths_eq && bytes_eq));

        for (size_t i = 0; i < old_str_len; ++i) {
            assert(((uint8_t *)str->bytes)[i] == old_str_bytes[i]);
        }
        for (size_t i = 0; i < old_buf_len; ++i) {
            assert(buf->buffer[i] == old_buf_buffer[i]);
        }
    }
}
