#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf src;
    uint8_t *nondet_buffer = ensure_nondet_buffer_is_allocated(MAX_BUFFER_SIZE);
    size_t nondet_len = nd_size_t();
    __CPROVER_assume(nondet_len <= MAX_BUFFER_SIZE);

    aws_byte_buf_init(&src, nondet_buffer, nondet_len);

    int result = aws_byte_buf_from_empty_array(&dest, allocator, &src);

    assert(result == AWS_OP_SUCCESS);
    assert(dest.len == 0);
    assert(dest.capacity > 0);
    assert(dest.allocator == allocator);
    assert(dest.buffer!= NULL);
    assert(aws_byte_buf_is_valid(&dest));
}
