#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    /* Ensure preconditions for valid execution */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Ensure memory is readable for non-zero lengths */
    __CPROVER_assume(a.len == 0 || AWS_MEM_IS_READABLE(a.ptr, a.len));
    __CPROVER_assume(b.len == 0 || AWS_MEM_IS_READABLE(b.buffer, b.len));

    /* Save old state */
    size_t old_a_len = a.len;
    uint8_t *old_a_ptr = a.ptr;
    size_t old_b_len = b.len;
    uint8_t *old_b_buffer = b.buffer;
    size_t old_b_capacity = b.capacity;
    void *old_b_allocator = b.allocator;

    /* Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* Assert unchanged fields (const arguments → no modification) */
    assert(a.len == old_a_len);
    assert(a.ptr == old_a_ptr);
    assert(b.len == old_b_len);
    assert(b.buffer == old_b_buffer);
    assert(b.capacity == old_b_capacity);
    assert(b.allocator == old_b_allocator);

    /* Assert validity invariants */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Behavioral correctness: result == (lengths equal AND bytes match) */
    if (a.len == b.len) {
        bool byte_match = true;
        for (size_t i = 0; i < a.len; i++) {
            if (a.ptr[i] != b.buffer[i]) {
                byte_match = false;
                break;
            }
        }
        if (byte_match) {
            assert(result);
        } else {
            assert(!result);
        }
    } else {
        assert(!result);
    }
}
