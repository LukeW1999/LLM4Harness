#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_eq_harness(void) {
    struct aws_byte_buf a;
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    bool result = aws_byte_buf_eq(&a, &b);

    (void)result; /* suppress unused variable warning */

    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.buffer == old_a.buffer);
    assert(a.allocator == old_a.allocator);

    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.buffer == old_b.buffer);
    assert(b.allocator == old_b.allocator);
}
