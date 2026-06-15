#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "aws/common/byte_buf.h"
#include "aws/common/allocator.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(allocator->mem_acquire != NULL);
    __CPROVER_assume(allocator->mem_release != NULL);
    __CPROVER_assume(allocator->mem_realloc != NULL);
    __CPROVER_assume(allocator->mem_calloc != NULL);

    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_byte_buf old = buf;

    int result = aws_byte_buf_init(&buf, allocator, capacity);

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
