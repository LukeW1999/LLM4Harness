#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
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

    struct aws_byte_buf old_dest = dest;

    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args > 0);

    struct aws_byte_buf *buffers[number_of_args];
    for (size_t i = 0; i < number_of_args; i++) {
        buffers[i] = (struct aws_byte_buf *)malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buffers[i] != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(buffers[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buffers[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(buffers[i]));
    }

    int result = aws_byte_buf_cat(&dest, number_of_args, buffers[0]);
    for (size_t i = 1; i < number_of_args; i++) {
        result = aws_byte_buf_cat(&dest, number_of_args, buffers[i]);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len >= old_dest.len);
    } else {
        assert(dest.len == old_dest.len);
    }

    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    assert(aws_byte_buf_is_valid(&dest));

    for (size_t i = 0; i < number_of_args; i++) {
        free(buffers[i]);
    }
}
