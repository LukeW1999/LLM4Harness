#include <assert.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf = {0};

    size_t capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_allocator *alloc = aws_default_allocator();

    int init_result = aws_byte_buf_init(&buf, alloc, capacity);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    buf.len = (size_t)nondet_uint64_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    bool zero = nondet_uint8_t() ? true : false;

    aws_byte_buf_reset(&buf, zero);

    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == alloc);
    if (capacity == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer != NULL);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
