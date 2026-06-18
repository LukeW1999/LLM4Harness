#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf buf = {0};

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);

    aws_byte_buf_from_empty_array(&buf, allocator, capacity);

    if (buf.buffer != NULL) {
        assert(buf.capacity == capacity);
        assert(buf.len == 0);
    } else {
        assert(buf.capacity == 0);
    }
}
