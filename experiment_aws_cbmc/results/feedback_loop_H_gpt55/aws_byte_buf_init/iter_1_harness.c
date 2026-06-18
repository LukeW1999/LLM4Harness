#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity = nondet_size_t();

    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_allocator *old_allocator_arg = allocator;
    size_t old_capacity_arg = capacity;

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    assert(allocator == old_allocator_arg);
    assert(capacity == old_capacity_arg);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
            assert(AWS_MEM_IS_WRITABLE(buf.buffer, capacity));
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(capacity != 0);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
