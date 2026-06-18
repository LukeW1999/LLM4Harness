#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_cat_harness(void) {
    /* Buffers used as sources */
    struct aws_byte_buf buffer1;
    struct aws_byte_buf buffer2;
    struct aws_byte_buf buffer3;

    /* Destination buffer */
    struct aws_byte_buf dest;

    /* Allocate and nondet‑initialize buffers */
    ensure_byte_buf_is_allocated(&buffer1, MAX_BUFFER_SIZE);
    ensure_byte_buf_is_allocated(&buffer2, MAX_BUFFER_SIZE);
    ensure_byte_buf_is_allocated(&buffer3, MAX_BUFFER_SIZE);
    ensure_byte_buf_is_allocated(&dest,    MAX_BUFFER_SIZE);

    /* Structural validity assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer1));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer2, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer2));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer3, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer3));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Lengths must be within capacities */
    __CPROVER_assume(buffer1.len <= buffer1.capacity);
    __CPROVER_assume(buffer2.len <= buffer2.capacity);
    __CPROVER_assume(buffer3.len <= buffer3.capacity);
    __CPROVER_assume(dest.len   <= dest.capacity);

    /* Non‑null buffers when length > 0 */
    __CPROVER_assume(buffer1.buffer != NULL || buffer1.len == 0);
    __CPROVER_assume(buffer2.buffer != NULL || buffer2.len == 0);
    __CPROVER_assume(buffer3.buffer != NULL || buffer3.len == 0);
    __CPROVER_assume(dest.buffer    != NULL || dest.capacity == 0);

    /* Ensure buffers have non‑zero capacity to avoid zero‑size malloc issues */
    __CPROVER_assume(dest.capacity > 0);
    __CPROVER_assume(buffer1.capacity > 0);
    __CPROVER_assume(buffer2.capacity > 0);
    __CPROVER_assume(buffer3.capacity > 0);

    /* Additional preconditions to avoid overlapping buffers */
    __CPROVER_assume(dest.buffer != NULL);
    __CPROVER_assume(buffer1.buffer != NULL);
    __CPROVER_assume(buffer2.buffer != NULL);
    __CPROVER_assume(buffer3.buffer != NULL);
    __CPROVER_assume(dest.buffer != buffer1.buffer);
    __CPROVER_assume(dest.buffer != buffer2.buffer);
    __CPROVER_assume(dest.buffer != buffer3.buffer);
    __CPROVER_assume(buffer1.buffer != buffer2.buffer);
    __CPROVER_assume(buffer1.buffer != buffer3.buffer);
    __CPROVER_assume(buffer2.buffer != buffer3.buffer);

    /* Choose a nondet number of arguments (must be 2 or 3) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= 3);

    /* Ensure destination has enough capacity for the concatenation */
    size_t total_src_len = buffer1.len + buffer2.len;
    if (number_of_args == 3) {
        total_src_len += buffer3.len;
    }
    __CPROVER_assume(dest.capacity - dest.len >= total_src_len);

    /* Snapshot of pre‑state for frame condition checks */
    struct aws_byte_buf dest_old = dest;
    uint8_t *dest_buf_old = NULL;
    if (dest.buffer != NULL) {
        dest_buf_old = malloc(dest.capacity);
        __CPROVER_assume(dest_buf_old != NULL);
        __CPROVER_assume(dest_buf_old != dest.buffer);
        memcpy(dest_buf_old, dest.buffer, dest.capacity);
    }

    struct aws_byte_buf buf1_old = buffer1;
    uint8_t *buf1_buf_old = NULL;
    if (buffer1.buffer != NULL) {
        buf1_buf_old = malloc(buffer1.capacity);
        __CPROVER_assume(buf1_buf_old != NULL);
        __CPROVER_assume(buf1_buf_old != buffer1.buffer);
        memcpy(buf1_buf_old, buffer1.buffer, buffer1.capacity);
    }

    struct aws_byte_buf buf2_old = buffer2;
    uint8_t *buf2_buf_old = NULL;
    if (buffer2.buffer != NULL) {
        buf2_buf_old = malloc(buffer2.capacity);
        __CPROVER_assume(buf2_buf_old != NULL);
        __CPROVER_assume(buf2_buf_old != buffer2.buffer);
        memcpy(buf2_buf_old, buffer2.buffer, buffer2.capacity);
    }

    struct aws_byte_buf buf3_old = buffer3;
    uint8_t *buf3_buf_old = NULL;
    if (buffer3.buffer != NULL) {
        buf3_buf_old = malloc(buffer3.capacity);
        __CPROVER_assume(buf3_buf_old != NULL);
        __CPROVER_assume(buf3_buf_old != buffer3.buffer);
        memcpy(buf3_buf_old, buffer3.buffer, buffer3.capacity);
    }

    int ret;
    if (number_of_args == 2) {
        ret = aws_byte_buf_cat(&dest, number_of_args, &buffer1, &buffer2);
    } else {
        ret = aws_byte_buf_cat(&dest, number_of_args, &buffer1, &buffer2, &buffer3);
    }

    /* 1. Return value must be a valid AWS operation code */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* 2. Destination buffer invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(dest.len <= dest.capacity);

    if (dest.buffer != NULL && dest_buf_old != NULL && dest.buffer == dest_old.buffer) {
        /* Existing data up to the original length must be unchanged */
        assert(memcmp(dest.buffer, dest_buf_old, dest_old.len) == 0);
    }

    /* 3. Source buffers must be unchanged (frame condition) */
    assert(aws_byte_buf_is_valid(&buffer1));
    assert(buffer1.len == buf1_old.len);
    assert(buffer1.capacity == buf1_old.capacity);
    assert(buffer1.buffer == buf1_old.buffer);
    if (buffer1.buffer != NULL && buf1_buf_old != NULL) {
        assert(memcmp(buffer1.buffer, buf1_buf_old, buffer1.capacity) == 0);
    }

    assert(aws_byte_buf_is_valid(&buffer2));
    assert(buffer2.len == buf2_old.len);
    assert(buffer2.capacity == buf2_old.capacity);
    assert(buffer2.buffer == buf2_old.buffer);
    if (buffer2.buffer != NULL && buf2_buf_old != NULL) {
        assert(memcmp(buffer2.buffer, buf2_buf_old, buffer2.capacity) == 0);
    }

    assert(aws_byte_buf_is_valid(&buffer3));
    assert(buffer3.len == buf3_old.len);
    assert(buffer3.capacity == buf3_old.capacity);
    assert(buffer3.buffer == buf3_old.buffer);
    if (buffer3.buffer != NULL && buf3_buf_old != NULL) {
        assert(memcmp(buffer3.buffer, buf3_buf_old, buffer3.capacity) == 0);
    }

    /* Clean up auxiliary allocations */
    free(dest_buf_old);
    free(buf1_buf_old);
    free(buf2_buf_old);
    free(buf3_buf_old);
}
