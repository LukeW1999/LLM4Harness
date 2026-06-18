#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic string */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;
    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= 1024);
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);
        str->allocator = allocator;
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    }

    /* nondeterministic byte buffer */
    bool buf_is_null = nondet_bool();
    struct aws_byte_buf *buf = NULL;
    if (!buf_is_null) {
        size_t buf_len = nondet_size_t();
        __CPROVER_assume(buf_len <= 1024);
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        buf->allocator = allocator;
        buf->capacity = buf_len;
        buf->len = buf_len;
        buf->buffer = malloc(buf_len);
        __CPROVER_assume(buf->buffer != NULL);
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
    uint8_t *old_buf_buffer = NULL;
    if (buf) {
        old_buf_len = buf->len;
        old_buf_buffer = buf->buffer;
    }

    /* call under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* ASSERT_POSTCONDITIONS_HERE */
}
