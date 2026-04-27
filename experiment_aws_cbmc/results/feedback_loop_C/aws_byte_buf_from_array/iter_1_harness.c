// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init_copy returns AWS_OP_SUCCESS:
//   - dest.buffer: CHANGES to a new allocated buffer
//   - dest.len: CHANGES to src.len
//   - dest.capacity: CHANGES to src.capacity
//   - dest.allocator: CHANGES to allocator
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init_copy returns AWS_OP_ERR:
//   - dest.buffer: CHANGES to NULL
//   - dest.len: CHANGES to 0
//   - dest.capacity: CHANGES to 0
//   - dest.allocator: CHANGES to allocator
//
// === STEP 3: FRAME CONDITIONS ===
// dest (struct aws_byte_buf):
//   - buffer: CHANGED on success, CHANGED on failure
//   - len: CHANGED on success, CHANGED on failure
//   - capacity: CHANGED on success, CHANGED on failure
//   - allocator: CHANGED always
// src (struct aws_byte_buf):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&dest): YES (must hold after call)
//   - aws_byte_buf_is_valid(&src): YES (must hold before call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = can_fail_allocator_new();

    // Initialize src with bounded values
    size_t max_src_capacity = MAX_BUFFER_SIZE;
    src.buffer = bounded_malloc(max_src_capacity);
    src.len = nondet_size_t();
    src.capacity = max_src_capacity;
    src.allocator = allocator;

    // Ensure src is valid
    assume(aws_byte_buf_is_valid(&src));

    // Save old state of dest
    struct aws_byte_buf old_dest = dest;

    // Call the function under test
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    // Step 1: Success Path
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        assert(dest.allocator == allocator);
        assert(aws_byte_buf_is_valid(&dest));
    }
    // Step 2: Failure Path
    else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
        assert(aws_byte_buf_is_valid(&dest));
    }

    // Step 3: Frame Conditions
    assert(src.buffer == old_dest.buffer);
    assert(src.len == old_dest.len);
    assert(src.capacity == old_dest.capacity);
    assert(src.allocator == old_dest.allocator);

    // Step 4: Validity Invariants
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    // Clean up
    if (dest.buffer) {
        aws_mem_release(allocator, dest.buffer);
    }
    if (src.buffer) {
        aws_mem_release(allocator, src.buffer);
    }
    aws_allocator_destroy(allocator);
}
