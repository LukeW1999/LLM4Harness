#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/pointer_utils.h>
#include <proof_helpers/memory_helpers.h>

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    size_t capacity = nondet_size_t();

    // Initialize allocator non-deterministically
    ensure_allocated_data_is_readable(allocator, sizeof(*allocator));

    // Preconditions
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE); // Assuming MAX_BUFFER_SIZE is defined somewhere

    // Save old state for postconditions
    struct aws_byte_buf old_buf = buf;

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        assert(aws_byte_buf_is_valid(&buf));
        if (capacity > 0) {
            assert(AWS_MEM_IS_WRITABLE(buf.buffer, capacity));
        } else {
            assert(buf.buffer == NULL);
        }
    } else if (result == AWS_OP_ERR) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == allocator);
        assert(!aws_byte_buf_is_valid(&buf));
    } else {
        assert(0); // Unreachable
    }

    // Check that inputs are not modified
    assert(allocator == old_buf.allocator);
}
