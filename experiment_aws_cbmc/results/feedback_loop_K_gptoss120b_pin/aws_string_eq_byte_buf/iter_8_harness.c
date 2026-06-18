#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEN 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str = NULL;
    uint8_t str_buf[MAX_LEN];

    if (__CPROVER_nondet_bool()) {
        size_t len = (size_t)__CPROVER_nondet_ulong();
        __CPROVER_assume(len <= MAX_LEN);
        for (size_t i = 0; i < len; ++i) {
            str_buf[i] = __CPROVER_nondet_uchar();
        }
        str = aws_string_new_from_array(aws_default_allocator(), str_buf, len);
    }

    struct aws_byte_buf *buf = NULL;
    struct aws_byte_buf buf_obj;
    uint8_t buf_buf[MAX_LEN];

    if (__CPROVER_nondet_bool()) {
        size_t capacity = (size_t)__CPROVER_nondet_ulong();
        __CPROVER_assume(capacity <= MAX_LEN);
        size_t len = (size_t)__CPROVER_nondet_ulong();
        __CPROVER_assume(len <= capacity);
        for (size_t i = 0; i < capacity; ++i) {
            buf_buf[i] = __CPROVER_nondet_uchar();
        }
        buf_obj.allocator = aws_default_allocator();
        buf_obj.capacity = capacity;
        buf_obj.len = len;
        buf_obj.buffer = (capacity > 0) ? buf_buf : NULL;
        buf = &buf_obj;
    }

    /* Preserve original state */
    size_t str_len_orig = 0;
    uint8_t str_bytes_orig[MAX_LEN];
    if (str) {
        str_len_orig = str->len;
        if (str_len_orig > 0) {
            memcpy(str_bytes_orig, str->bytes, str_len_orig);
        }
    }

    struct aws_byte_buf buf_orig;
    uint8_t buf_buf_orig[MAX_LEN];
    if (buf) {
        buf_orig = *buf;
        if (buf->capacity > 0) {
            memcpy(buf_buf_orig, buf->buffer, buf->capacity);
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
        assert(str->len == str_len_orig);
        if (str_len_orig > 0) {
            assert(memcmp(str->bytes, str_bytes_orig, str_len_orig) == 0);
        }
    }

    if (buf) {
        assert(buf->len == buf_orig.len);
        assert(buf->capacity == buf_orig.capacity);
        assert(buf->buffer == buf_orig.buffer);
        if (buf->capacity > 0) {
            assert(memcmp(buf->buffer, buf_buf_orig, buf->capacity) == 0);
        }
    }
}
