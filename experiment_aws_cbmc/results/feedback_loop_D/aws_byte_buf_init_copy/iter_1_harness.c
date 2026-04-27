// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init_copy returns AWS_OP_SUCCESS (or the successful value):
//   - dest.len: CHANGES to src->len
//   - dest.buffer: CHANGES to new allocated buffer
//   - dest.capacity: CHANGES to src->capacity
//   - dest.allocator: CHANGES to allocator
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init_copy returns AWS_OP_ERR (or fails):
//   - dest.len: UNCHANGED
//   - dest.buffer: SET to NULL
//   - dest.capacity: UNCHANGED
//   - dest.allocator: SET to allocator
//
// === STEP 3: FRAME CONDITIONS ===
// dest (struct aws_byte_buf):
//   - len: CHANGED on success, UNCHANGED on failure
//   - buffer: CHANGED on success, SET to NULL on failure
//   - capacity: CHANGED on success, UNCHANGED on failure
//   - allocator: CHANGED always
// src (struct aws_byte_buf):
//   - len: UNCHANGED always
//   - buffer: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// allocator (struct aws_allocator):
//   - All fields: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&dest): YES (must hold after call)
//   - aws_byte_buf_is_valid(&src): YES/NO (not modified, so depends on initial state)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator allocator;

    // Initialize src with arbitrary values
    src.len = nondet_size_t();
    src.capacity = nondet_size_t();
    src.buffer = can_fail_malloc(src.capacity);
    src.allocator = &allocator;

    // Initialize dest with arbitrary values
    dest.len = nondet_size_t();
    dest.capacity = nondet_size_t();
    dest.buffer = can_fail_malloc(dest.capacity);
    dest.allocator = &allocator;

    // Store old state of dest
    struct aws_byte_buf old_dest = dest;

    // Assume src is valid
    assume(aws_byte_buf_is_valid(&src));

    // Call the function under test
    int result = aws_byte_buf_init_copy(&dest, &allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        // Assertions for success path
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        assert(dest.allocator == &allocator);
        assert(dest.buffer != NULL);
        assert_bytes_match(dest.buffer, src.buffer, src.len);
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        // Assertions for failure path
        assert(dest.len == old_dest.len);
        assert(dest.buffer == NULL);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == &allocator);
        assert(aws_byte_buf_is_valid(&dest));
    }
}
