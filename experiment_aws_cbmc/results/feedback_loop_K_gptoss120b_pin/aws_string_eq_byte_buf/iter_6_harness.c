#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEN 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str = NULL;
    struct aws_string str_obj;
    uint8_t str_buf[MAX_LEN];

    if (__CPROVER_nondet_bool()) {
        size_t len = (size_t)__CPROVER_nondet_ulong();
        __CPROVER_assume(len <= MAX_LEN);
        str = &str_obj;
        str->allocator = aws_default_allocator();
        str->len = len;
        str->bytes = str_buf;
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = __CPROVER_nondet_uchar();
        }
    }

    struct aws_byte_buf *buf = NULL;
    struct aws_byte_buf buf_obj;
    uint8_t buf_buf[MAX_LEN];

    if (__CPROVER_nondet_bool()) {
        size_t capacity = (size_t)__CPROVER_nondet_ulong();
        __CPROVER_assume(capacity <= MAX_LEN);
        size_t len = (size_t)__CPROVER_nondet_ulong();
        __CPROVER_assume(len <= capacity);
        buf = &buf_obj;
        buf->allocator = aws_default_allocator();
        buf->capacity = capacity;
        buf->len = len;
        if (capacity > 0) {
            buf->buffer = buf_buf;
            for (size_t i = 0; i < capacity; ++i) {
                buf->buffer[i] = __CPROVER_nondet_uchar();
            }
        } else {
            buf->buffer = NULL;
        }
    }

    /* Preserve original state */
    struct aws_string str_orig;
    uint8_t str_buf_orig[MAX_LEN];
    if (str) {
        memcpy(&str_orig, str, sizeof(str_orig));
        memcpy(str_buf_orig, str_buf, MAX_LEN);
    }

    struct aws_byte_buf buf_orig;
    uint8_t buf_buf_orig[MAX_LEN];
    if (buf) {
        memcpy(&buf_orig, buf, sizeof(buf_orig));
        if (buf->capacity > 0) {
            memcpy(buf_buf_orig, buf_buf, MAX_LEN);
        }
    }

    bool result = aws_string_eq_byte_buf(str, buf);

    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        bool expected = (str->len == buf->len) &&
                        (memcmp(str->bytes, buf->buffer, str->len) == 0);
        assert(result == expected);
    }

    if (str) {
        assert(str->len == str_orig.len);
        assert(str->bytes == str_orig.bytes);
        assert(memcmp(str, &str_orig, sizeof(str_orig)) == 0);
        assert(memcmp(str_buf, str_buf_orig, MAX_LEN) == 0);
    }

    if (buf) {
        assert(buf->len == buf_orig.len);
        assert(buf->capacity == buf_orig.capacity);
        assert(buf->buffer == buf_orig.buffer);
        if (buf->capacity > 0) {
            assert(memcmp(buf->buffer, buf_buf_orig, MAX_LEN) == 0);
        }
    }
}
