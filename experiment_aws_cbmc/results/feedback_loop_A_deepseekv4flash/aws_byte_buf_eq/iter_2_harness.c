#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness() {
    /* Non-deterministic flag: whether buffers should be equal */
    bool equal = nondet_bool();

    /* Create two byte buffers */
    struct aws_byte_buf a, b;

    /* Bounding and validity preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old states for immutability checks */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* Condition buffers according to equality flag */
    if (equal) {
        /* Make buffers identical */
        b = a;
    } else {
        /* Force buffers to be different by making lengths unequal */
        __CPROVER_assume(a.len != b.len);
    }

    /* Save state of b before call (equal case overwrote it) */
    struct aws_byte_buf b_before_call = b;

    /* Call the function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* Postconditions: result matches expected equality */
    if (equal) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Immutability of a */
    assert(aws_byte_buf_is_valid(&a));
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);
    assert(a.buffer == old_a.buffer);
    assert_bytes_match(a.buffer, old_a.buffer, a.len);

    /* Immutability of b (with respect to its state before the call) */
    assert(aws_byte_buf_is_valid(&b));
    assert(b.len == b_before_call.len);
    assert(b.capacity == b_before_call.capacity);
    assert(b.allocator == b_before_call.allocator);
    assert(b.buffer == b_before_call.buffer);
    assert_bytes_match(b.buffer, b_before_call.buffer, b.len);
}
