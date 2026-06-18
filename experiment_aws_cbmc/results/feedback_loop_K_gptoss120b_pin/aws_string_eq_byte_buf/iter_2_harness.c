#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>

#include "proof_helpers/make_common_data_structures.h"

#define MAX_LEN 256

void aws_string_eq_byte_buf_harness(void) {
    /* nondet decide whether to have a string */
    struct aws_string *str = NULL;
    if (__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= MAX_LEN);
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        if (str) {
            str->allocator = aws_default_allocator();
            str->len = len;
            str->bytes = (const uint8_t *)(str + 1);
            /* fill bytes with nondet values */
            for (size_t i = 0; i < len; ++i) {
                ((uint8_t *)str->bytes)[i] = __CPROVER_nondet_uchar();
            }
        }
    }

    /* nondet decide whether to have a byte buffer */
    struct aws_byte_buf *buf = NULL;
    if (__CPROVER_nondet_bool()) {
        size_t capacity = __CPROVER_nondet_uint();
        __CPROVER_assume(capacity <= MAX_LEN);
        buf = (struct aws_byte_buf *)malloc(sizeof(struct aws_byte_buf));
        if (buf) {
            buf->allocator = aws_default_allocator();
            buf->capacity = capacity;
            buf->len = __CPROVER_nondet_uint();
            __CPROVER_assume(buf->len <= buf->capacity);
            if (capacity > 0) {
                buf->buffer = (uint8_t *)malloc(capacity);
                if (buf->buffer) {
                    for (size_t i = 0; i < capacity; ++i) {
                        buf->buffer[i] = __CPROVER_nondet_uchar();
                    }
                } else {
                    buf->capacity = 0;
                    buf->len = 0;
                }
            } else {
                buf->buffer = NULL;
            }
        }
    }

    /* Preserve original state */
    struct aws_string *str_orig = NULL;
    size_t str_len_orig = 0;
    const uint8_t *str_bytes_orig = NULL;
    if (str) {
        str_len_orig = str->len;
        str_bytes_orig = str->bytes;
        str_orig = (struct aws_string *)malloc(sizeof(struct aws_string) + str_len_orig);
        assert(str_orig);
        memcpy(str_orig, str, sizeof(struct aws_string) + str_len_orig);
    }

    struct aws_byte_buf *buf_orig = NULL;
    size_t buf_len_orig = 0;
    size_t buf_capacity_orig = 0;
    uint8_t *buf_buffer_orig = NULL;
    if (buf) {
        buf_len_orig = buf->len;
        buf_capacity_orig = buf->capacity;
        buf_orig = (struct aws_byte_buf *)malloc(sizeof(struct aws_byte_buf));
        assert(buf_orig);
        memcpy(buf_orig, buf, sizeof(struct aws_byte_buf));
        if (buf->buffer && buf->capacity > 0) {
            buf_buffer_orig = (uint8_t *)malloc(buf->capacity);
            assert(buf_buffer_orig);
            memcpy(buf_buffer_orig, buf->buffer, buf->capacity);
        }
    }

    /* Call function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postcondition */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        bool expected = (str->len == buf->len) &&
                        (memcmp(str->bytes, buf->buffer, str->len) == 0);
        assert(result == expected);
    }

    /* Frame conditions */
    if (str) {
        assert(str->len == str_len_orig);
        assert(str->bytes == str_bytes_orig);
        assert(memcmp(str, str_orig, sizeof(struct aws_string) + str_len_orig) == 0);
    }
    if (buf) {
        assert(buf->len == buf_len_orig);
        assert(buf->capacity == buf_capacity_orig);
        if (buf->buffer && buf->capacity > 0) {
            assert(memcmp(buf->buffer, buf_buffer_orig, buf->capacity) == 0);
        }
    }

    /* Clean up */
    if (str) free((void *)str);
    if (buf) {
        if (buf->buffer) free(buf->buffer);
        free(buf);
    }
    if (str_orig) free(str_orig);
    if (buf_orig) free(buf_orig);
    if (buf_buffer_orig) free(buf_buffer_orig);
}
