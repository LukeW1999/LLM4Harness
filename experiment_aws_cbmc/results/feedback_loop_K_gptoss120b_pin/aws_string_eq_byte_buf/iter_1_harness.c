#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>

#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_byte_buf_harness(void) {
    /* Allocate (or leave NULL) a nondeterministic aws_string */
    struct aws_string *str;
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    /* Allocate (or leave NULL) a nondeterministic aws_byte_buf */
    struct aws_byte_buf *buf;
    __CPROVER_assume(buf == NULL || aws_byte_buf_is_valid(buf));

    /* Preserve original state for frame condition checks */
    struct aws_string *str_orig = NULL;
    size_t str_len_orig = 0;
    const uint8_t *str_bytes_orig = NULL;

    struct aws_byte_buf *buf_orig = NULL;
    size_t buf_len_orig = 0;
    size_t buf_capacity_orig = 0;
    uint8_t *buf_buffer_orig = NULL;

    if (str != NULL) {
        str_orig = (struct aws_string *)malloc(sizeof(struct aws_string) + str->len);
        assert(str_orig != NULL);
        memcpy((void *)str_orig, (void *)str, sizeof(struct aws_string) + str->len);
        str_len_orig = str->len;
        str_bytes_orig = str->bytes;
    }

    if (buf != NULL) {
        buf_orig = (struct aws_byte_buf *)malloc(sizeof(struct aws_byte_buf));
        assert(buf_orig != NULL);
        memcpy(buf_orig, buf, sizeof(struct aws_byte_buf));
        buf_len_orig = buf->len;
        buf_capacity_orig = buf->capacity;
        if (buf->buffer != NULL && buf->capacity > 0) {
            buf_buffer_orig = (uint8_t *)malloc(buf->capacity);
            assert(buf_buffer_orig != NULL);
            memcpy(buf_buffer_orig, buf->buffer, buf->capacity);
        }
    }

    /* Call the function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postcondition: return value matches specification */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        bool expected = (str->len == buf->len) &&
                        (memcmp(str->bytes, buf->buffer, str->len) == 0);
        assert(result == expected);
    }

    /* Frame condition: str unchanged */
    if (str != NULL) {
        assert(str->len == str_len_orig);
        assert(str->bytes == str_bytes_orig);
        assert(memcmp(str, str_orig, sizeof(struct aws_string) + str_len_orig) == 0);
    }

    /* Frame condition: buf unchanged */
    if (buf != NULL) {
        assert(buf->len == buf_len_orig);
        assert(buf->capacity == buf_capacity_orig);
        if (buf->buffer != NULL && buf->capacity > 0) {
            assert(memcmp(buf->buffer, buf_buffer_orig, buf->capacity) == 0);
        }
    }

    /* Clean up */
    if (str_orig) free((void *)str_orig);
    if (buf_orig) free(buf_orig);
    if (buf_buffer_orig) free(buf_buffer_orig);

    return 0;
}
