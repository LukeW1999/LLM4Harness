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

    /* Structural validity assumptions (must be copied verbatim) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer1));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer2, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer2));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer3, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer3));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Snapshot of pre‑state for frame condition checks */
    struct aws_byte_buf dest_old = dest;
    uint8_t *dest_buf_old = NULL;
    if (dest.buffer != NULL) {
        dest_buf_old = malloc(dest.capacity);
        __CPROVER_assume(dest_buf_old != NULL);
        memcpy(dest_buf_old, dest.buffer, dest.capacity);
    }

    struct aws_byte_buf buf1_old = buffer1;
    uint8_t *buf1_buf_old = NULL;
    if (buffer1.buffer != NULL) {
        buf1_buf_old = malloc(buffer1.capacity);
        __CPROVER_assume(buf1_buf_old != NULL);
        memcpy(buf1_buf_old, buffer1.buffer, buffer1.capacity);
    }

    struct aws_byte_buf buf2_old = buffer2;
    uint8_t *buf2_buf_old = NULL;
    if (buffer2.buffer != NULL) {
        buf2_buf_old = malloc(buffer2.capacity);
        __CPROVER_assume(buf2_buf_old != NULL);
        memcpy(buf2_buf_old, buffer2.buffer, buffer2.capacity);
    }

    struct aws_byte_buf buf3_old = buffer3;
    uint8_t *buf3_buf_old = NULL;
    if (buffer3.buffer != NULL) {
        buf3_buf_old = malloc(buffer3.capacity);
        __CPROVER_assume(buf3_buf_old != NULL);
        memcpy(buf3_buf_old, buffer3.buffer, buffer3.capacity);
    }

    /* Choose a nondet number of arguments (must be >1) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= 3);

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
    assert(dest.buffer == dest_old.buffer);          /* buffer pointer unchanged */
    assert(dest.capacity == dest_old.capacity);      /* capacity unchanged */

    if (dest.buffer != NULL && dest_buf_old != NULL) {
        /* Existing data up to the original length must be unchanged */
        assert(memcmp(dest.buffer, dest_buf_old, dest_old.len) == 0);
        /* Bytes beyond the (possibly) new length must remain unchanged */
        if (dest.capacity > dest_old.len) {
            size_t unchanged_start = dest.capacity > dest.len ? dest.len : dest.capacity;
            assert(memcmp(dest.buffer + unchanged_start,
                          dest_buf_old + unchanged_start,
                          dest.capacity - unchanged_start) == 0);
        }
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

    return 0;
}
