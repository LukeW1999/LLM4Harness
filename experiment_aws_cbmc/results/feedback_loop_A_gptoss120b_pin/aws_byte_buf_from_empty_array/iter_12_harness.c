#include <assert.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity;
    __CPROVER_assume(capacity <= 1024);

    aws_byte_buf_from_empty_array(&buf, allocator, capacity);

    assert(buf.allocator == allocator);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    if (capacity == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer != NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
