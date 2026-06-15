#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    struct aws_byte_buf buf;
    uint8_t data[1024];
    size_t len = nondet_size_t();

    __CPROVER_assume(len > 0);
    __CPROVER_assume(len <= 1024);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    int result = aws_byte_buf_init_from_array(&buf, allocator, data, len);
    __CPROVER_assume(result == AWS_OP_SUCCESS);

    assert(buf.allocator == allocator);
    assert(buf.capacity == len);
    assert(buf.len == len);
    assert(buf.buffer != NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
