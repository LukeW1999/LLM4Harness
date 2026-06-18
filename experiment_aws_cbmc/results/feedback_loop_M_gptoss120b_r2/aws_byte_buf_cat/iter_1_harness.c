#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness(void) {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* source buffers (at least two as required by the API) */
    struct aws_byte_buf src1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    struct aws_byte_buf src2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    /* Number of arguments: exactly 2 (minimum required) */
    size_t number_of_args = 2;

    /* Call function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args, &src1, &src2);

    /* Post‑conditions for the destination buffer */
    /* allocator, buffer pointer and capacity must never change */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);

    /* length must be non‑decreasing and cannot exceed the sum of source lengths */
    assert(dest.len >= old_dest.len);
    assert(dest.len <= old_dest.len + src1.len + src2.len);

    if (result == AWS_OP_SUCCESS) {
        /* On success all source data must have been appended */
        assert(dest.len == old_dest.len + src1.len + src2.len);
    } else {
        /* On failure the destination may contain a prefix of the concatenated data */
        /* No additional constraints beyond the generic ones above */
    }

    /* Source buffers must remain unchanged */
    assert(src1.len == old_src1.len);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.allocator == old_src1.allocator);
    assert(src2.len == old_src2.len);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.buffer == old_src2.buffer);
    assert(src2.allocator == old_src2.allocator);

    /* Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
}
