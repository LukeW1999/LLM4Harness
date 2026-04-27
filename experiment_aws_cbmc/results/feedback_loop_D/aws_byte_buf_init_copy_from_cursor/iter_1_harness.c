// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init_copy_from_cursor returns AWS_OP_SUCCESS (or the successful value):
//   - dest.buffer: CHANGES to allocated memory or NULL if src.len is 0
//   - dest.len: CHANGES to src.len
//   - dest.capacity: CHANGES to src.len
//   - dest.allocator: CHANGES to allocator

// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init_copy_from_cursor returns AWS_OP_ERR (or fails):
//   - dest.buffer: UNCHANGED
//   - dest.len: UNCHANGED
//   - dest.capacity: UNCHANGED
//   - dest.allocator: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   dest (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success, UNCHANGED on failure
//   allocator (struct aws_allocator, if any):
//     - allocation functions: UNCHANGED always
//   src (struct aws_byte_cursor):
//     - ptr: UNCHANGED always
//     - len: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&src): YES (must hold before call)
//   - aws_byte_buf_is_valid(&dest): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    struct aws_byte_cursor src;

    // Assume preconditions
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    // Save old state
    struct aws_byte_buf old_dest = dest;

    // Call function under test
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != old_dest.buffer || src.len == 0); // buffer changes unless src.len is 0
        assert(dest.len == src.len); // len changes
        assert(dest.capacity == src.len); // capacity changes
        assert(dest.allocator == allocator); // allocator changes
        if (src.len > 0) {
            assert_bytes_match(dest.buffer, src.ptr, src.len); // contents match
        }
    } else { // Assertions for failure path
        assert(dest.buffer == old_dest.buffer); // buffer unchanged
        assert(dest.len == old_dest.len); // len unchanged
        assert(dest.capacity == old_dest.capacity); // capacity unchanged
        assert(dest.allocator == old_dest.allocator); // allocator unchanged
    }

    // Validity invariants
    assert(aws_byte_cursor_is_valid(&src)); // src remains valid
    assert(aws_byte_buf_is_valid(&dest)); // dest is valid after call
}
