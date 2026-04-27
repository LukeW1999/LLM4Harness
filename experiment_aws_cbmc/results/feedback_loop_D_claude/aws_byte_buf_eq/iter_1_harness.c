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
//   - return value: true (contents are equal)
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
// param a (struct aws_byte_buf *):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// param b (struct aws_byte_buf *):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(a): YES (postcondition in implementation)
//   - aws_byte_buf_is_valid(b): YES (postcondition in implementation)
//   - The function calls aws_array_eq which uses memcmp when len > 0
//   - Returns true iff a->len == b->len AND memcmp(a->buffer, b->buffer, a->len) == 0

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_byte_buf_eq_harness(void) {
    /* Allocate and set up two aws_byte_buf structs */
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    /* Ensure both buffers have allocated buffer members */
    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    /* Assume both are valid (precondition) */
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Bound the sizes to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    /* Save old state */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* Call the function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* === FRAME CONDITIONS: both structs must be unchanged === */
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* === VALIDITY INVARIANTS === */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* === CORRECTNESS: result must match expected semantics === */
    /* If result is true, lengths must be equal */
    if (result) {
        assert(a.len == b.len);
    }

    /* If lengths differ, result must be false */
    if (a.len != b.len) {
        assert(!result);
    }

    /* If both have zero length, result must be true */
    if (a.len == 0 && b.len == 0) {
        assert(result);
    }
}
