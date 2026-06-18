#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministically choose whether str is NULL or a valid string */
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* Allocate enough memory for the header and maximum possible bytes */
        str = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        /* Bound the length */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str->len = len;
        /* Use the default allocator (or NULL, both are valid) */
        str->allocator = aws_default_allocator();
        /* Assume the string is valid (includes memory readability) */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministically choose whether buf_ptr is NULL or points to a valid byte_buf */
    const struct aws_byte_buf *buf_ptr;
    if (nondet_bool()) {
        buf_ptr = NULL;
    } else {
        struct aws_byte_buf *buf_heap = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf_heap != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(buf_heap, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf_heap);
        __CPROVER_assume(aws_byte_buf_is_valid(buf_heap));
        buf_ptr = buf_heap;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Compute the expected result */
    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        if (str->len != buf_ptr->len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; i++) {
                if (str->bytes[i] != buf_ptr->buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }

    /* Assert the function returns the correct boolean */
    assert(result == expected);

    /* Assert that valid structures remain valid (no modification) */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr != NULL) {
        assert(aws_byte_buf_is_valid(buf_ptr));
    }
}
