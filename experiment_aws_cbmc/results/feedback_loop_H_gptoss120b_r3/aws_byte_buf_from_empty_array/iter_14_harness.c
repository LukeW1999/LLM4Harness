#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    struct aws_byte_buf buf = {0};
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity;
    __CPROVER_assume(capacity <= 1024);

    int result = aws_byte_buf_from_empty_array(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.allocator == allocator);
        assert(buf.capacity == capacity);
        assert(buf.len == 0);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
