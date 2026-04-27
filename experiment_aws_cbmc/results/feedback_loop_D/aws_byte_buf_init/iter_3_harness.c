#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize buf with arbitrary values
    buf.buffer = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = allocator;

    size_t capacity = nondet_size_t();

    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(buf.buffer != NULL || capacity == 0);
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        assert(aws_byte_buf_is_valid(&buf));
    }
    // Failure path assertions
    else if (result == AWS_OP_ERR) {
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == allocator);
        assert(aws_byte_buf_is_valid(&buf));
    }

    // Free allocated memory
    free(buf.buffer);
}
