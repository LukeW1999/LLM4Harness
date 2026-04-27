// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_eq returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When aws_byte_buf_eq returns true:
//   - a->buffer: UNCHANGED
//   - a->len: UNCHANGED
//   - a->capacity: UNCHANGED
//   - a->allocator: UNCHANGED
//   - b->buffer: UNCHANGED
//   - b->len: UNCHANGED
//   - b->capacity: UNCHANGED
//   - b->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_eq returns false:
//   - a->buffer: UNCHANGED
//   - a->len: UNCHANGED
//   - a->capacity: UNCHANGED
//   - a->allocator: UNCHANGED
//   - b->buffer: UNCHANGED
//   - b->len: UNCHANGED
//   - b->capacity: UNCHANGED
//   - b->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// param a (struct aws_byte_buf):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// param b (struct aws_byte_buf):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&a): YES (must hold after call, precondition)
//   - aws_byte_buf_is_valid(&b): YES (must hold after call, precondition)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_eq_harness(void) {
    /* Allocate two aws_byte_buf structures */
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    /* Ensure they have allocated buffer members with bounded sizes */
    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    /* Assume validity preconditions */
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Bound the sizes for tractability */
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    /* Save old state */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* Call the function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* === Frame condition assertions === */
    /* a is unchanged */
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);

    /* b is unchanged */
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* === Validity invariants === */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* === Correctness assertions === */
    /* If result is true, then a.len == b.len and contents match */
    if (result) {
        assert(a.len == b.len);
        if (a.len > 0) {
            assert_bytes_match(a.buffer, b.buffer, a.len);
        }
    }

    /* If a.len != b.len, result must be false */
    if (a.len != b.len) {
        assert(!result);
    }
}
