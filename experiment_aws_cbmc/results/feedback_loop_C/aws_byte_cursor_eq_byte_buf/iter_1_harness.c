// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_eq_byte_buf returns true:
//   - a.ptr: UNCHANGED
//   - a.len: UNCHANGED
//   - b.buffer: UNCHANGED
//   - b.len: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_eq_byte_buf returns false:
//   - a.ptr: UNCHANGED
//   - a.len: UNCHANGED
//   - b.buffer: UNCHANGED
//   - b.len: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   a (struct aws_byte_cursor):
//     - ptr: UNCHANGED always
//     - len: UNCHANGED always
//   b (struct aws_byte_buf):
//     - buffer: UNCHANGED always
//     - len: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&a): YES (must hold after call)
//   - aws_byte_buf_is_valid(&b): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    // Initialize a and b with arbitrary values
    size_t max_size = MAX_BUFFER_SIZE;
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);
    a.len = nondet_size_t();
    b.len = nondet_size_t();
    b.capacity = nondet_size_t();
    b.allocator = (struct aws_allocator *)nondet_ptr();

    // Ensure a and b are valid initially
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    // Save old state
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    // Call the function
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    // Assert frame conditions and validity invariants
    assert(a.ptr == old_a.ptr); // UNCHANGED always
    assert(a.len == old_a.len); // UNCHANGED always
    assert(b.buffer == old_b.buffer); // UNCHANGED always
    assert(b.len == old_b.len); // UNCHANGED always
    assert(b.capacity == old_b.capacity); // UNCHANGED always
    assert(b.allocator == old_b.allocator); // UNCHANGED always

    assert(aws_byte_cursor_is_valid(&a)); // aws_byte_cursor_is_valid(&a): YES (must hold after call)
    assert(aws_byte_buf_is_valid(&b)); // aws_byte_buf_is_valid(&b): YES (must hold after call)
}
