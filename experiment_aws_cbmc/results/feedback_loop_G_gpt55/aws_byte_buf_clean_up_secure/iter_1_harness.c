#include <aws/common/byte_buf.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();

    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    int init_result = aws_byte_buf_init(&buf, aws_default_allocator(), capacity);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    assert(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;
    (void)old;

    aws_byte_buf_clean_up_secure(&buf);

    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
