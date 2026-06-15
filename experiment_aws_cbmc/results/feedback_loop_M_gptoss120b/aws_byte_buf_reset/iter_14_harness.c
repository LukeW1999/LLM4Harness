#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>

void aws_byte_buf_reset_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf buf;

    size_t capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(capacity <= (size_t)1024);

    int init_res = aws_byte_buf_init(&buf, allocator, capacity);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    aws_byte_buf_reset(&buf);

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
