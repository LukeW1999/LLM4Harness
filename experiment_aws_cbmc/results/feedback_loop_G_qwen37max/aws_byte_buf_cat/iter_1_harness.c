#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    struct aws_byte_buf arg1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&arg1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&arg1);
    __CPROVER_assume(aws_byte_buf_is_valid(&arg1));

    struct aws_byte_buf arg2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&arg2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&arg2);
    __CPROVER_assume(aws_byte_buf_is_valid(&arg2));

    struct aws_byte_buf old_dest = dest;

    /* The Doxygen specifies "Number of args must be greater than 1", so we test with 2 args */
    int result = aws_byte_buf_cat(&dest, 2, &arg1, &arg2);

    /* Unchanged fields (implied invariants) */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    /* Postconditions for success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_dest.len + arg1.len + arg2.len);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
}
