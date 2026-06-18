#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 4096);

    aws_byte_buf_from_empty_array(&buf, allocator, capacity);

    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == allocator);
    if (capacity == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer != NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
