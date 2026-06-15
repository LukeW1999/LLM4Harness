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
    /* Destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Source buffers (exactly two, as required >1) */
    struct aws_byte_buf src1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    struct aws_byte_buf src2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    /* Call function under test with exactly two source buffers */
    int result = aws_byte_buf_cat(&dest, 2, &src1, &src2);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Destination buffer length increased by total source lengths */
        assert(dest.len == old_dest.len + src1.len + src2.len);
        /* Destination buffer pointer, capacity, allocator unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        /* Source buffers unchanged */
        assert(src1.len == old_src1.len);
        assert(src1.capacity == old_src1.capacity);
        assert(src1.buffer == old_src1.buffer);
        assert(src1.allocator == old_src1.allocator);
        assert(src2.len == old_src2.len);
        assert(src2.capacity == old_src2.capacity);
        assert(src2.buffer == old_src2.buffer);
        assert(src2.allocator == old_src2.allocator);
    } else {
        /* On failure, destination unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        /* Source buffers unchanged */
        assert(src1.len == old_src1.len);
        assert(src1.capacity == old_src1.capacity);
        assert(src1.buffer == old_src1.buffer);
        assert(src1.allocator == old_src1.allocator);
        assert(src2.len == old_src2.len);
        assert(src2.capacity == old_src2.capacity);
        assert(src2.buffer == old_src2.buffer);
        assert(src2.allocator == old_src2.allocator);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
}
