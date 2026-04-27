// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init returns AWS_OP_SUCCESS (or the successful value):
//   - buf.buffer: CHANGES to allocated memory or NULL if capacity is 0
//   - buf.len: CHANGES to 0
//   - buf.capacity: CHANGES to the input capacity
//   - buf.allocator: CHANGES to the input allocator
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init returns AWS_OP_ERR (or fails):
//   - buf.buffer: CHANGES to NULL
//   - buf.len: CHANGES to 0
//   - buf.capacity: CHANGES to 0
//   - buf.allocator: CHANGES to NULL
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: CHANGED on success, CHANGED on failure
//     - len: CHANGED on success, CHANGED on failure
//     - capacity: CHANGED on success, CHANGED on failure
//     - allocator: CHANGED on success, CHANGED on failure
//   allocator (struct aws_allocator, if any):
//     - All fields: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)
//   - aws_XXX_is_valid(&allocator): NO (not applicable as allocator is not checked for validity)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = can_fail_allocator_new();

    // Save old state
    struct aws_byte_buf old_buf = buf;

    // Initialize buf with arbitrary values
    buf.buffer = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));

    size_t capacity = nondet_size_t();

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(buf.buffer == (capacity == 0 ? NULL : old_buf.buffer));
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        assert(aws_byte_buf_is_valid(&buf));
    }
    // Failure path assertions
    else if (result == AWS_OP_ERR) {
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
        assert(aws_byte_buf_is_valid(&buf));
    }

    // Frame condition assertions
    assert(allocator == can_fail_allocator_new());

    // Free allocated memory
    free(buf.buffer);
    free(buf.allocator);
}
