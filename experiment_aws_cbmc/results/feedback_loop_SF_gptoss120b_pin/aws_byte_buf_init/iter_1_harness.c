#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "aws/common/byte_buf.h"
#include "aws/common/allocator.h"
#include "aws/common/memory.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_harness(void) {
    /* Symbolic inputs */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity = nondet_size_t();

    /* Precondition assumptions */
    __CPROVER_assume(&buf != NULL);
    __CPROVER_assume(allocator != NULL);

    /* Snapshot of input state */
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* Return value must be either success (0) or error (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Buffer must be initialized correctly */
        assert(buf.allocator == allocator);
        assert(buf.capacity == capacity);
        assert(buf.len == 0);
        if (capacity > 0) {
            assert(buf.buffer != NULL);
        } else {
            assert(buf.buffer == NULL);
        }
    } else {
        /* On error, the buffer must remain unchanged */
        assert(buf.len == old_len);
        assert(buf.capacity == old_capacity);
        assert(buf.buffer == old_buffer);
        assert(buf.allocator == old_allocator);
    }
}
