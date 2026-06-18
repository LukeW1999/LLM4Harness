#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "aws/common/byte_buf.h"
#include "aws/common/assert.h"
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 64

void aws_byte_buf_append_harness(void) {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* allocate concrete memory for destination buffer */
    if (to.capacity > 0) {
        uint8_t *buf = malloc(to.capacity);
        __CPROVER_assume(buf != NULL);
        /* copy existing contents (if any) */
        if (to.len > 0) {
            memcpy(buf, to.buffer, to.len);
        }
        to.buffer = buf;
    } else {
        to.buffer = NULL;
    }

    /* allocate concrete memory for source cursor */
    if (from.len > 0) {
        uint8_t *src = malloc(from.len);
        __CPROVER_assume(src != NULL);
        from.ptr = src;
    } else {
        from.ptr = NULL;
    }

    /* ensure source and destination do not overlap */
    if (to.buffer != NULL && from.ptr != NULL) {
        __CPROVER_assume(
            (size_t)from.ptr + from.len <= (size_t)to.buffer ||
            (size_t)to.buffer + to.capacity <= (size_t)from.ptr);
    }

    /* capture pre‑state */
    size_t old_len = to.len;
    size_t old_capacity = to.capacity;

    uint8_t *old_buffer = NULL;
    if (to.buffer != NULL && old_len > 0) {
        old_buffer = malloc(old_capacity);
        __CPROVER_assume(old_buffer != NULL);
        memcpy(old_buffer, to.buffer, old_len);
    }

    uint8_t *old_from_ptr = NULL;
    if (from.ptr != NULL && from.len > 0) {
        old_from_ptr = malloc(from.len);
        __CPROVER_assume(old_from_ptr != NULL);
        memcpy(old_from_ptr, from.ptr, from.len);
    }

    /* call the function under test */
    int ret = aws_byte_buf_append(&to, &from);

    /* 1. return value / error code correctness */
    __CPROVER_assert(
        (to.capacity - old_len < from.len) ?
            ret == AWS_ERROR_SHORT_BUFFER :
            ret == AWS_OP_SUCCESS,
        "return value matches capacity condition");

    /* 2. post‑condition validity predicates */
    __CPROVER_assert(aws_byte_buf_is_valid(&to), "to remains a valid aws_byte_buf");
    __CPROVER_assert(aws_byte_cursor_is_valid(&from), "from remains a valid aws_byte_cursor");

    /* 3. length invariants */
    if (to.capacity - old_len >= from.len) {
        __CPROVER_assert(to.len == old_len + from.len,
                         "to.len increased by from.len on success");
    } else {
        __CPROVER_assert(to.len == old_len,
                         "to.len unchanged on failure");
    }

    /* 4. frame conditions for to.buffer (pre‑existing data unchanged) */
    if (to.buffer != NULL && old_buffer != NULL) {
        for (size_t i = 0; i < old_len; ++i) {
            __CPROVER_assert(to.buffer[i] == old_buffer[i],
                             "pre‑existing data in to.buffer unchanged");
        }
    }

    /* 5. frame conditions for from.ptr (must remain unchanged) */
    if (from.ptr != NULL && old_from_ptr != NULL) {
        for (size_t i = 0; i < from.len; ++i) {
            __CPROVER_assert(from.ptr[i] == old_from_ptr[i],
                             "data in from.ptr unchanged");
        }
    }

    /* clean up */
    free(old_buffer);
    free(old_from_ptr);
    if (to.buffer != NULL) free(to.buffer);
    if (from.ptr != NULL) free(from.ptr);
}
