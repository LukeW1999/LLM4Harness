#include <assert.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);

void aws_byte_buf_init_harness(void) {
    struct aws_byte_buf buf;

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t capacity = nondet_size_t();

    int ret = aws_byte_buf_init(&buf, allocator, capacity);

    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);
    assert(aws_byte_buf_is_valid(&buf));

    if (ret == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }
}
