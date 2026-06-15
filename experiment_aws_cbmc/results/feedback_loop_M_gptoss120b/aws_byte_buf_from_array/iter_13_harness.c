#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_from_array_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t len = nondet_size_t();
    __CPROVER_assume(len > 0);
    __CPROVER_assume(len <= 1024);

    uint8_t *data = aws_mem_acquire(allocator, len);
    __CPROVER_assume(data != NULL);
    for (size_t i = 0; i < len; ++i) {
        data[i] = nondet_uint8_t();
    }

    struct aws_byte_buf buf;
    aws_byte_buf_init_from_array(&buf, allocator, data, len);

    assert(buf.len == len);
    assert(buf.capacity >= len);
    assert(buf.buffer != NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
