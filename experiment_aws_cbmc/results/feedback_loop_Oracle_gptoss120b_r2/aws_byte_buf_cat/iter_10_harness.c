#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

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

    /* Distinct allocated regions (no overlap) */
    __CPROVER_assume(buffer1.buffer != buffer2.buffer);
    __CPROVER_assume(buffer1.buffer != buffer3.buffer);
    __CPROVER_assume(buffer2.buffer != buffer3.buffer);
    __CPROVER_assume(dest.buffer    != buffer1.buffer);
    __CPROVER_assume(dest.buffer    != buffer2.buffer);
    __CPROVER_assume(dest.buffer    != buffer3.buffer);

    /* Stronger non‑overlap: whole destination region does not intersect any source region */
    __CPROVER_assume(dest.buffer + dest.capacity <= buffer1.buffer ||
                     buffer1.buffer + buffer1.capacity <= dest.buffer);
    __CPROVER_assume(dest.buffer + dest.capacity <= buffer2.buffer ||
                     buffer2.buffer + buffer2.capacity <= dest.buffer);
    __CPROVER_assume(dest.buffer + dest.capacity <= buffer3.buffer ||
                     buffer3.buffer + buffer3.capacity <= dest.buffer);

    /* Choose 2 or 3 source buffers */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= 3);

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
