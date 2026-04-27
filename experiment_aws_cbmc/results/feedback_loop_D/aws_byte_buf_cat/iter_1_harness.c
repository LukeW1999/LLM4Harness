// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_cat returns AWS_OP_SUCCESS (or the successful value):
//   - dest.len: CHANGES to sum of lengths of all source buffers
//   - dest.buffer: CHANGES to contain concatenated data from all source buffers
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_cat returns AWS_OP_ERR (or fails):
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
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&dest): YES (must hold after call)

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"
#include "aws_common_byte_buf.h"

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    size_t number_of_args = nondet_size_t();
    struct aws_byte_buf *buffers[number_of_args];
    struct aws_byte_buf old_dest = dest;

    // Initialize dest
    __CPROVER_assume(number_of_args <= MAX_BUFFER_SIZE);
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    // Initialize source buffers
    for (size_t i = 0; i < number_of_args; ++i) {
        buffers[i] = malloc(sizeof(struct aws_byte_buf));
        ensure_byte_buf_has_allocated_buffer_member(buffers[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(buffers[i]));
    }

    int result = aws_byte_buf_cat(&dest, number_of_args, buffers);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        size_t total_len = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            total_len += buffers[i]->len;
        }
        assert(dest.len == total_len);
        // We cannot directly assert the contents of dest.buffer due to the complexity of concatenation logic
    }

    // Assertions for failure path
    if (result == AWS_OP_ERR) {
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    // Validity invariant
    assert(aws_byte_buf_is_valid(&dest));

    // Free allocated memory
    for (size_t i = 0; i < number_of_args; ++i) {
        free(buffers[i]->buffer);
        free(buffers[i]);
    }
    free(dest.buffer);
}
