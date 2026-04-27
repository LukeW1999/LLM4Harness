// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_cat returns AWS_OP_SUCCESS (or the successful value):
//   - dest.len: CHANGES to sum of lengths of all source buffers
//   - dest.buffer: CHANGES to contain concatenated contents of all source buffers
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

#include <aws/common/byte_buf.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>
#include <cbmc_proof/nondet.h>
#include <cbmc_proof/proof_allocators.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args > 0 && number_of_args <= MAX_BUFFER_SIZE / sizeof(struct aws_byte_buf *));
    
    // Initialize dest with a bounded buffer
    size_t dest_capacity = nondet_size_t();
    __CPROVER_assume(dest_capacity <= MAX_BUFFER_SIZE);
    dest.buffer = bounded_malloc(dest_capacity);
    dest.len = nondet_size_t();
    __CPROVER_assume(dest.len <= dest_capacity);
    dest.capacity = dest_capacity;
    dest.allocator = NULL;

    // Create an array of source byte buffers
    struct aws_byte_buf *sources = bounded_malloc(number_of_args * sizeof(struct aws_byte_buf *));
    for (size_t i = 0; i < number_of_args; ++i) {
        sources[i] = malloc(sizeof(struct aws_byte_buf));
        sources[i]->buffer = bounded_malloc(nondet_size_t());
        sources[i]->len = nondet_size_t();
        sources[i]->capacity = nondet_size_t();
        sources[i]->allocator = NULL;
    }

    // Save old state of dest
    struct aws_byte_buf old_dest = dest;

    // Call the function under test
    int result = aws_byte_buf_cat(&dest, number_of_args, (struct aws_byte_buf **)&sources);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        // Check that dest.len is the sum of lengths of all source buffers
        size_t total_len = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            total_len += sources[i]->len;
        }
        assert(dest.len == total_len);

        // Check that dest.buffer contains concatenated contents of all source buffers
        size_t offset = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            assert_bytes_match(dest.buffer + offset, sources[i]->buffer, sources[i]->len);
            offset += sources[i]->len;
        }
    } else {
        // Assertions for failure path
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    // Check validity of dest
    assert(aws_byte_buf_is_valid(&dest));

    // Free allocated memory
    for (size_t i = 0; i < number_of_args; ++i) {
        free(sources[i]->buffer);
        free(sources[i]);
    }
    free(sources);
    free(dest.buffer);
}
