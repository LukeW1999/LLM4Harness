#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);

    int result = aws_byte_buf_init(&buf, alloc, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.allocator == alloc);
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
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
