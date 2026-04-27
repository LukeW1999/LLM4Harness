#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_eq_harness() {
    /* data structures */
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    /* allocate and assume bounded */
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* save old states for immutability checks */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* assertions for both success and failure */
    if (result) {
        assert_bytes_match(a.buffer, b.buffer, a.len);
        assert(a.len == b.len);
    } else {
        assert(a.len != b.len || !aws_byte_cursor_eq_ignore_case(aws_byte_cursor_from_buf(&a), aws_byte_cursor_from_buf(&b)));
    }

    /* assertions for unchanged fields */
    assert(a.allocator == old_a.allocator);
    assert(a.buffer == old_a.buffer);
    assert(a.capacity == old_a.capacity);
    assert(a.len == old_a.len);

    assert(b.allocator == old_b.allocator);
    assert(b.buffer == old_b.buffer);
    assert(b.capacity == old_b.capacity);
    assert(b.len == old_b.len);

    /* assertions for validity invariants */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
