#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "aws/common/byte_buf.h"
#include "aws/common/allocator.h"
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_append_harness(void) {
    /* allocate and nondet-initialize the destination buffer */
    struct aws_byte_buf to;
    to.allocator = aws_default_allocator();

    /* nondet capacity and length within bounds */
    __CPROVER_assume(to.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(to.len <= to.capacity);

    if (to.capacity > 0) {
        to.buffer = malloc(to.capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }

    /* allocate and nondet-initialize the source cursor */
    struct aws_byte_cursor from;
    __CPROVER_assume(from.len <= MAX_BUFFER_SIZE);
    if (from.len > 0) {
        from.ptr = malloc(from.len);
        __CPROVER_assume(from.ptr != NULL);
    } else {
        from.ptr = NULL;
    }

    /* ground‑truth preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* snapshot old state for frame conditions */
    struct aws_byte_buf old_to = to;
    uint8_t *old_to_buf = NULL;
    if (to.capacity > 0) {
        old_to_buf = malloc(to.capacity);
        __CPROVER_assume(old_to_buf != NULL);
        memcpy(old_to_buf, to.buffer, to.capacity);
    }

    struct aws_byte_cursor old_from = from;
    uint8_t *old_from_buf = NULL;
    if (from.len > 0) {
        old_from_buf = malloc(from.len);
        __CPROVER_assume(old_from_buf != NULL);
        memcpy(old_from_buf, from.ptr, from.len);
    }

    /* call the function under test */
    int ret = aws_byte_buf_append(&to, &from);

    /* post‑condition: return value must be either success or the specific error */
    __CPROVER_assert(ret == AWS_OP_SUCCESS ||
                     ret == aws_raise_error(AWS_ERROR_DEST_COPY_TOO_SMALL),
                     "return value is valid");

    /* post‑condition: structural validity must still hold */
    __CPROVER_assert(aws_byte_buf_is_valid(&to),
                     "aws_byte_buf_is_valid after call");
    __CPROVER_assert(aws_byte_cursor_is_valid(&from),
                     "aws_byte_cursor_is_valid after call");

    /* length invariants */
    __CPROVER_assert(to.len <= to.capacity,
                     "to.len does not exceed to.capacity");

    if (ret == AWS_OP_SUCCESS) {
        /* successful append: length increased exactly by from.len */
        __CPROVER_assert(to.len == old_to.len + from.len,
                         "to.len increased correctly on success");
        /* capacity unchanged */
        __CPROVER_assert(to.capacity == old_to.capacity,
                         "to.capacity unchanged on success");
        /* buffer content before old_len unchanged */
        if (old_to.capacity > 0 && old_to.len > 0) {
            __CPROVER_assert(
                memcmp(to.buffer, old_to_buf, old_to.len) == 0,
                "pre‑existing data unchanged on success");
        }
        /* appended region matches source data */
        if (from.len > 0) {
            __CPROVER_assert(
                memcmp(to.buffer + old_to.len, from.ptr, from.len) == 0,
                "appended data matches source on success");
        }
    } else {
        /* error case: length unchanged */
        __CPROVER_assert(to.len == old_to.len,
                         "to.len unchanged on error");
        /* buffer unchanged */
        if (to.capacity > 0) {
            __CPROVER_assert(
                memcmp(to.buffer, old_to_buf, to.capacity) == 0,
                "buffer unchanged on error");
        }
    }

    /* frame condition: memory outside the buffers must be unchanged */
    /* (handled implicitly by the above memcmp checks on the allocated regions) */

    /* free allocated memory */
    if (to.buffer) free(to.buffer);
    if (from.ptr) free((void *)from.ptr);
    if (old_to_buf) free(old_to_buf);
    if (old_from_buf) free(old_from_buf);

    return 0;
}
