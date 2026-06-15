#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_cat_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    dest.allocator = aws_default_allocator();

    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args > 0);

    struct aws_byte_buf *buffers[number_of_args];
    for (size_t i = 0; i < number_of_args; i++) {
        buffers[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(aws_byte_buf_is_bounded(buffers[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buffers[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(buffers[i]));
        buffers[i]->allocator = aws_default_allocator();
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args, buffers);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len >= old_dest.len);
    } else {
        assert(dest.len == old_dest.len);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == aws_default_allocator());
    assert(dest.buffer == old_dest.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));

    for (size_t i = 0; i < number_of_args; i++) {
        free(buffers[i]);
    }
}
