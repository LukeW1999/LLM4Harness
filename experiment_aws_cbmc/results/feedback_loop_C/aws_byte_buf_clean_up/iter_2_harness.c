#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;
    size_t buffer_size = nondet_size_t();
    buf.buffer = (uint8_t *)malloc(buffer_size);
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();

    struct aws_allocator *allocator = aws_default_allocator();
    buf.allocator = allocator;

    struct aws_byte_buf old_buf = buf;

    // Assume preconditions
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(allocator->mem_release != NULL);

    aws_byte_buf_clean_up(&buf);

    // Check frame conditions and validity invariants
    if (old_buf.allocator != NULL && old_buf.buffer != NULL) {
        // Success path
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    } else {
        // Failure path
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
