#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf buf = {0};
    size_t capacity = nondet_size_t();

    __CPROVER_assume(capacity <= 1024);

    int result = aws_byte_buf_from_empty_array(&buf, allocator, capacity);

    if (result == 0) {
        assert(buf.capacity == capacity);
        assert(buf.len == 0);
        assert(buf.buffer != NULL);
    } else {
        assert(buf.buffer == NULL || buf.capacity == 0);
    }
}
