#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness(void) {
    /* 1. Declare and set up the first buffer */
    struct aws_byte_buf a;
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    /* 2. Declare and set up the second buffer */
    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 3. Save old states to check immutability */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* 5. Assert postconditions */
    /* Result must be consistent with lengths */
    if (a.len != b.len) {
        assert(result == false);
    }

    /* 6. a must not have been modified */
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);

    /* 7. b must not have been modified */
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* 8. validity invariants still hold */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
