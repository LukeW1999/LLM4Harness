#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* Use the default allocator for all allocations */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate (or not) a nondet aws_string */
    struct aws_string *str = NULL;
    if (__CPROVER_nondet_bool()) {
        str = make_aws_string(allocator);
        __CPROVER_assume(aws_string_is_valid(str));
        /* Ensure the string's allocator field is set */
        str->allocator = allocator;
    }

    /* Allocate (or not) a nondet aws_byte_buf */
    struct aws_byte_buf *buf = NULL;
    if (__CPROVER_nondet_bool()) {
        buf = make_aws_byte_buf(allocator);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
        /* Ensure the byte buffer's allocator field is set */
        buf->allocator = allocator;
    }

    /* Preserve copies of inputs for frame checking */
    struct aws_string *str_orig = str;
    struct aws_byte_buf *buf_orig = buf;

    size_t str_len_orig = (str != NULL) ? str->len : 0;
    size_t buf_len_orig = (buf != NULL) ? buf->len : 0;

    /* Deep copies of the underlying byte arrays */
    uint8_t *str_bytes_copy = NULL;
    if (str != NULL && str->len > 0) {
        str_bytes_copy = malloc(str->len);
        __CPROVER_assume(str_bytes_copy != NULL);
        memcpy(str_bytes_copy, str->bytes, str->len);
    }

    uint8_t *buf_bytes_copy = NULL;
    if (buf != NULL && buf->len > 0) {
        buf_bytes_copy = malloc(buf->len);
        __CPROVER_assume(buf_bytes_copy != NULL);
        memcpy(buf_bytes_copy, buf->buffer, buf->len);
    }

    /* Call the function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postcondition: result matches the specification */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        bool expected = false;
        if (str->len == buf->len) {
            if (str->len == 0) {
                expected = true;
            } else {
                expected = (memcmp(str->bytes, buf->buffer, str->len) == 0);
            }
        }
        assert(result == expected);
    }

    /* Frame conditions: inputs unchanged */
    if (str != NULL) {
        assert(str->len == str_len_orig);
        if (str->len > 0) {
            assert(memcmp(str->bytes, str_bytes_copy, str->len) == 0);
        }
    }

    if (buf != NULL) {
        assert(buf->len == buf_len_orig);
        if (buf->len > 0) {
            assert(memcmp(buf->buffer, buf_bytes_copy, buf->len) == 0);
        }
    }

    /* Clean up */
    if (str_bytes_copy) free(str_bytes_copy);
    if (buf_bytes_copy) free(buf_bytes_copy);
    if (str_orig) aws_string_destroy(str_orig);
    if (buf_orig) aws_byte_buf_clean_up(buf_orig);
}
