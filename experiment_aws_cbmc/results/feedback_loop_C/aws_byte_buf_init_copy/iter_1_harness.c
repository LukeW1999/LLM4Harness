// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init_copy returns AWS_OP_SUCCESS (or the successful value):
//   - dest.len: CHANGES to src.len
//   - dest.buffer: CHANGES to newly allocated memory with contents copied from src.buffer
//   - dest.capacity: CHANGES to src.capacity
//   - dest.allocator: CHANGES to allocator
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init_copy returns AWS_OP_ERR (or fails):
//   - dest.len: UNCHANGED
//   - dest.buffer: UNCHANGED
//   - dest.capacity: UNCHANGED
//   - dest.allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   dest (struct aws_byte_buf):
//     - len: CHANGED on success, UNCHANGED on failure
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success, UNCHANGED on failure
//   allocator (struct aws_allocator, if any):
//     - alloc: UNCHANGED always
//     - free: UNCHANGED always
//   src (struct aws_byte_buf, if any):
//     - len: UNCHANGED always
//     - buffer: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&dest): YES (must hold after call)
//   - aws_byte_buf_is_valid(&allocator): NO (allocator is not checked for validity)
//   - aws_byte_buf_is_valid(&src): YES (must hold before call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator allocator;
    struct aws_byte_buf src;

    // Initialize src with arbitrary but valid values
    src.len = nondet_size_t();
    src.capacity = nondet_size_t();
    src.buffer = (uint8_t *)malloc(src.capacity);
    __CPROVER_assume(src.buffer != NULL || src.capacity == 0); // Ensure buffer is NULL if capacity is 0
    src.allocator = &allocator;

    // Initialize allocator with arbitrary but valid values
    allocator.alloc = malloc;
    allocator.free = free;

    // Save old state of dest
    struct aws_byte_buf old_dest = dest;

    // Call the function under test
    int result = aws_byte_buf_init_copy(&dest, &allocator, &src);

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        assert(dest.allocator == &allocator);
        if (src.buffer) {
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else { // Failure path assertions
        assert(aws_byte_buf_is_valid(&old_dest));
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    // Free allocated memory
    if (src.buffer) {
        free(src.buffer);
    }
    if (dest.buffer && result == AWS_OP_SUCCESS) {
        free(dest.buffer);
    }
}
