#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 100

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;

    buf.allocator = aws_default_allocator();
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    buf.buffer = malloc(buf.capacity);
    __CPROVER_assume(buf.buffer != NULL);
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    bool zero;
    __CPROVER_assume(zero == false || zero == true);

    aws_byte_buf_reset(&buf, zero);

    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    if (zero && buf.buffer != NULL) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }
}
