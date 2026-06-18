#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t capacity = (size_t)nondet_uint();

    __CPROVER_assume(capacity <= 1024);

    int result = aws_byte_buf_from_empty_array(&buf, alloc, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.allocator == alloc);
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
