#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    uint8_t *nondet_array = ensure_nondet_bytes_are_zero_terminated(MAX_BUFFER_SIZE);
    size_t nondet_len = nd_size_t();
    __CPROVER_assume(nondet_len <= MAX_BUFFER_SIZE);

    int result = aws_byte_buf_from_array(&dest, allocator, nondet_array, nondet_len);

    assert(result == AWS_OP_SUCCESS || result == AWS_ERROR_INVALID_ARGUMENT);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == nondet_len);
        assert(dest.capacity >= nondet_len);
        assert(dest.allocator == allocator);
        assert(memcmp(dest.buffer, nondet_array, nondet_len) == 0);
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
    }

    assert(dest.allocator == allocator || dest.allocator == NULL);

    assert(aws_byte_buf_is_valid(&dest));
}
