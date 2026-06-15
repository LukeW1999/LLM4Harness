#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1000
#endif

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
