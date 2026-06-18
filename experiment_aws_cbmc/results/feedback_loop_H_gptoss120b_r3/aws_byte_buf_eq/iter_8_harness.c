#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_eq_harness(void) {
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    bool result = aws_byte_buf_eq(&a, &b);

    bool expected;
    if (a.buffer == NULL && b.buffer == NULL) {
        expected = true;
    } else if (a.buffer == NULL || b.buffer == NULL) {
        expected = false;
    } else if (a.len != b.len) {
        expected = false;
    } else {
        expected = (memcmp(a.buffer, b.buffer, a.len) == 0);
    }

    assert(result == expected);

    /* Verify that the input buffers are unchanged */
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);
}
