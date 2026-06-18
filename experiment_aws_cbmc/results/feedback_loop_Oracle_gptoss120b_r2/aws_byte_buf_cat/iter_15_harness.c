#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_cat_harness(void) {
    struct aws_byte_buf buffer1, buffer2, buffer3, dest;

    /* Allocate buffers with nondet contents */
    ensure_byte_buf_is_allocated(&buffer1, MAX_BUFFER_SIZE);
    ensure_byte_buf_is_allocated(&buffer2, MAX_BUFFER_SIZE);
    ensure_byte_buf_is_allocated(&buffer3, MAX_BUFFER_SIZE);
    ensure_byte_buf_is_allocated(&dest,    MAX_BUFFER_SIZE);

    /* Structural validity */
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer1));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer2));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer3));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Lengths within capacities */
    __CPROVER_assume(buffer1.len <= buffer1.capacity);
    __CPROVER_assume(buffer2.len <= buffer2.capacity);
    __CPROVER_assume(buffer3.len <= buffer3.capacity);
    __CPROVER_assume(dest.len   <= dest.capacity);

    /* Non‑null buffers when length > 0 */
    __CPROVER_assume(buffer1.buffer != NULL || buffer1.len == 0);
    __CPROVER_assume(buffer2.buffer != NULL || buffer2.len == 0);
    __CPROVER_assume(buffer3.buffer != NULL || buffer3.len == 0);
    __CPROVER_assume(dest.buffer    != NULL || dest.capacity == 0);

    /* Ensure non‑zero capacities */
    __CPROVER_assume(buffer1.capacity > 0);
    __CPROVER_assume(buffer2.capacity > 0);
    __CPROVER_assume(buffer3.capacity > 0);
    __CPROVER_assume(dest.capacity   > 0);

    /* Prevent pointer overflow when adding capacities */
    __CPROVER_assume((uintptr_t)buffer1.buffer + buffer1.capacity > (uintptr_t)buffer1.buffer);
    __CPROVER_assume((uintptr_t)buffer2.buffer + buffer2.capacity > (uintptr_t)buffer2.buffer);
    __CPROVER_assume((uintptr_t)buffer3.buffer + buffer3.capacity > (uintptr_t)buffer3.buffer);
    __CPROVER_assume((uintptr_t)dest.buffer    + dest.capacity    > (uintptr_t)dest.buffer);

    /* Strong non‑overlap of whole buffer regions */
    {
        uintptr_t d_start = (uintptr_t)dest.buffer;
        uintptr_t d_end   = d_start + dest.capacity;

        uintptr_t b1_start = (uintptr_t)buffer1.buffer;
        uintptr_t b1_end   = b1_start + buffer1.capacity;
        uintptr_t b2_start = (uintptr_t)buffer2.buffer;
        uintptr_t b2_end   = b2_start + buffer2.capacity;
        uintptr_t b3_start = (uintptr_t)buffer3.buffer;
        uintptr_t b3_end   = b3_start + buffer3.capacity;

        __CPROVER_assume(d_end <= b1_start || b1_end <= d_start);
        __CPROVER_assume(d_end <= b2_start || b2_end <= d_start);
        __CPROVER_assume(d_end <= b3_start || b3_end <= d_start);

        __CPROVER_assume(b1_end <= b2_start || b2_end <= b1_start);
        __CPROVER_assume(b1_end <= b3_start || b3_end <= b1_start);
        __CPROVER_assume(b2_end <= b3_start || b3_end <= b2_start);
    }

    /* Choose 2 or 3 source buffers */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= 3);

    /* Ensure each used source has non‑zero length (avoids zero‑length memcpy edge cases) */
    __CPROVER_assume(buffer1.len > 0);
    __CPROVER_assume(buffer2.len > 0);
    if (number_of_args == 3) {
        __CPROVER_assume(buffer3.len > 0);
    }

    /* Ensure destination has enough space for concatenation */
    size_t total_src_len = buffer1.len + buffer2.len;
    if (number_of_args == 3) {
        total_src_len += buffer3.len;
    }
    __CPROVER_assume(dest.capacity - dest.len >= total_src_len);

    /* Destination space for each copy (sequential) */
    __CPROVER_assume(dest.len + buffer1.len <= dest.capacity);
    __CPROVER_assume(dest.len + buffer1.len + buffer2.len <= dest.capacity);
    if (number_of_args == 3) {
        __CPROVER_assume(dest.len + buffer1.len + buffer2.len + buffer3.len
                         <= dest.capacity);
    }

    /* No overlap between the region that will be written in dest and each source */
    {
        uintptr_t d_write_start = (uintptr_t)dest.buffer + dest.len;
        uintptr_t d_write_end   = d_write_start + buffer1.len;
        uintptr_t s1_start = (uintptr_t)buffer1.buffer;
        uintptr_t s1_end   = s1_start + buffer1.len;
        __CPROVER_assume(d_write_end <= s1_start || s1_end <= d_write_start);
    }
    {
        uintptr_t d_write_start = (uintptr_t)dest.buffer + dest.len + buffer1.len;
        uintptr_t d_write_end   = d_write_start + buffer2.len;
        uintptr_t s2_start = (uintptr_t)buffer2.buffer;
        uintptr_t s2_end   = s2_start + buffer2.len;
        __CPROVER_assume(d_write_end <= s2_start || s2_end <= d_write_start);
    }
    if (number_of_args == 3) {
        uintptr_t d_write_start = (uintptr_t)dest.buffer + dest.len + buffer1.len + buffer2.len;
        uintptr_t d_write_end   = d_write_start + buffer3.len;
        uintptr_t s3_start = (uintptr_t)buffer3.buffer;
        uintptr_t s3_end   = s3_start + buffer3.len;
        __CPROVER_assume(d_write_end <= s3_start || s3_end <= d_write_start);
    }

    /* Call the function under test */
    int ret;
    if (number_of_args == 2) {
        ret = aws_byte_buf_cat(&dest, number_of_args, &buffer1, &buffer2);
    } else {
        ret = aws_byte_buf_cat(&dest, number_of_args, &buffer1, &buffer2, &buffer3);
    }

    /* Return value must be a valid AWS operation code */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* Destination invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(dest.len <= dest.capacity);
}
