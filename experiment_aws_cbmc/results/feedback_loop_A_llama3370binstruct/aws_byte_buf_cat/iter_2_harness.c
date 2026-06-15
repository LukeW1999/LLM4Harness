#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args > 0);

    struct aws_byte_buf *buffers[number_of_args];
    for (size_t i = 0; i < number_of_args; i++) {
        buffers[i] = nondet_ptr();
        __CPROVER_assume(buffers[i]!= NULL);
        struct aws_byte_buf buffer = *buffers[i];
        __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buffer);
        __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    }

    struct aws_byte_buf old_dest = dest;

    int result = aws_byte_buf_cat(&dest, number_of_args, buffers);
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR_OOM);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len >= old_dest.len);
    } else {
        assert(dest.len == old_dest.len);
    }

    assert(dest.capacity >= old_dest.capacity);
    assert(dest.allocator == aws_default_allocator());
    assert(dest.buffer!= NULL);

    assert(aws_byte_buf_is_valid(&dest));
}
