#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/proof_allocators.h"

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = nondet_bool() ? NULL : nondet_allocator();

    /* Bounding and allocation for src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src (immutable) */
    struct aws_byte_buf old_src = src;

    /* Call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest is a copy of src */
        if (old_src.buffer == NULL) {
            assert(dest.len == 0);
            assert(dest.buffer == NULL);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            assert(dest.len == old_src.len);
            assert(dest.capacity == old_src.capacity);
            assert(dest.allocator == allocator);
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
        }
    } else {
        /* Failure: dest is zeroed (all fields 0) */
        assert(dest.len == 0);
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must remain unchanged in all cases */
    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* Validity invariant for dest */
    assert(aws_byte_buf_is_valid(&dest));
}
