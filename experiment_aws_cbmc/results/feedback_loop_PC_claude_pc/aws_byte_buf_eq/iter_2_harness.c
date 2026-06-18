#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness(void) {
    /* Set up two aws_byte_buf structs */
    struct aws_byte_buf a;
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state for frame conditions */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    struct store_byte_from_buffer a_byte_storage;
    if (a.buffer != NULL && a.len > 0) {
        save_byte_from_array(a.buffer, a.len, &a_byte_storage);
    }

    struct store_byte_from_buffer b_byte_storage;
    if (b.buffer != NULL && b.len > 0) {
        save_byte_from_array(b.buffer, b.len, &b_byte_storage);
    }

    /* Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* Postconditions */
    /* Result is a valid bool */
    assert(result == true || result == false);

    /* If lengths differ, must be false */
    if (a.len != b.len) {
        assert(result == false);
    }

    /* Frame: a and b must not be modified */
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* Assert byte contents unchanged */
    if (old_a.buffer != NULL && old_a.len > 0) {
        assert_byte_from_buffer_matches(a.buffer, &a_byte_storage);
    }
    if (old_b.buffer != NULL && old_b.len > 0) {
        assert_byte_from_buffer_matches(b.buffer, &b_byte_storage);
    }

    /* Invariants */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
