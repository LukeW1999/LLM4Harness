#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str = NULL;
    uint8_t str_mem[sizeof(struct aws_string) + MAX_STRING_LEN];

    if (!__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str = (struct aws_string *)str_mem;
        str->allocator = aws_default_allocator();
        str->len = len;
        uint8_t *bytes = (uint8_t *)(str_mem + sizeof(struct aws_string));
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = __CPROVER_nondet_uchar();
        }
    }

    struct aws_byte_buf *buf_ptr = NULL;
    struct aws_byte_buf buf;
    uint8_t buf_mem[MAX_STRING_LEN];

    if (!__CPROVER_nondet_bool()) {
        size_t capacity = __CPROVER_nondet_size_t();
        __CPROVER_assume(capacity <= MAX_STRING_LEN);
        buf.buffer = buf_mem;
        buf.capacity = capacity;
        buf.len = __CPROVER_nondet_size_t();
        __CPROVER_assume(buf.len <= capacity);
        for (size_t i = 0; i < buf.len; ++i) {
            buf.buffer[i] = __CPROVER_nondet_uchar();
        }
        buf_ptr = &buf;
    }

    /* Preserve old state */
    uint8_t old_str_bytes[MAX_STRING_LEN];
    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
        uint8_t *bytes = (uint8_t *)(str_mem + sizeof(struct aws_string));
        for (size_t i = 0; i < str->len; ++i) {
            old_str_bytes[i] = bytes[i];
        }
    }

    struct aws_byte_buf old_buf = {0};
    uint8_t old_buf_mem[MAX_STRING_LEN];
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
        if (buf_ptr->buffer != NULL && buf_ptr->len > 0) {
            for (size_t i = 0; i < buf_ptr->len; ++i) {
                old_buf_mem[i] = buf_ptr->buffer[i];
            }
            old_buf.buffer = old_buf_mem;
        }
    }

    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Compute expected result */
    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        expected = true;
        if (str->len != buf_ptr->len) {
            expected = false;
        } else {
            uint8_t *bytes = (uint8_t *)(str_mem + sizeof(struct aws_string));
            for (size_t i = 0; i < str->len; ++i) {
                if (bytes[i] != buf_ptr->buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);

    /* Verify immutability */
    if (str != NULL) {
        assert(old_str.allocator == str->allocator);
        assert(old_str.len == str->len);
        uint8_t *bytes = (uint8_t *)(str_mem + sizeof(struct aws_string));
        for (size_t i = 0; i < str->len; ++i) {
            assert(old_str_bytes[i] == bytes[i]);
        }
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        if (buf_ptr->len > 0) {
            for (size_t i = 0; i < buf_ptr->len; ++i) {
                assert(buf_ptr->buffer[i] == old_buf_mem[i]);
            }
        }
    }
}
