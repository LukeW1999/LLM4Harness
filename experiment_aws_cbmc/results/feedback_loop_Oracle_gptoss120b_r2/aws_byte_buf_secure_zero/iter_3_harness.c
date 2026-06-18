#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);

    if (aws_byte_buf_init(&buf, allocator, capacity) != AWS_OP_SUCCESS) {
        return;
    }

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    aws_byte_buf_secure_zero(&buf);

    assert(buf.capacity == capacity);
    assert(buf.len == 0);

    aws_byte_buf_clean_up(&buf);
}
