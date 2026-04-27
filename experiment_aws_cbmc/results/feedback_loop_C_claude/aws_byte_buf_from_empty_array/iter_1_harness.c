// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_from_empty_array always returns a struct (not an int), so there's no error path.
// The function always succeeds and returns a new aws_byte_buf.
// Return value:
//   - buf.buffer: SET to (uint8_t *)bytes if capacity > 0, else NULL
//   - buf.len: SET to 0
//   - buf.capacity: SET to capacity
//   - buf.allocator: SET to NULL
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns a valid aws_byte_buf.
//
// === STEP 3: FRAME CONDITIONS ===
// Return value (struct aws_byte_buf):
//   - buffer: SET to bytes if capacity > 0, else NULL
//   - len: SET to 0
//   - capacity: SET to capacity parameter
//   - allocator: SET to NULL
// Input parameters (bytes, capacity) are not modified (passed by value/pointer).
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&result): YES (must hold after call)
//   - The postcondition states aws_byte_buf_is_valid(&buf)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Non-deterministic inputs */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Allocate a writable buffer of the given capacity */
    uint8_t *bytes = NULL;
    if (capacity > 0) {
        bytes = malloc(capacity);
        __CPROVER_assume(bytes != NULL);
    }

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_empty_array(bytes, capacity);

    /* === Assertions for Step 1 (success path) === */
    /* buf.len must be 0 */
    assert(result.len == 0);

    /* buf.capacity must equal the capacity parameter */
    assert(result.capacity == capacity);

    /* buf.allocator must be NULL */
    assert(result.allocator == NULL);

    /* buf.buffer must be bytes if capacity > 0, else NULL */
    if (capacity > 0) {
        assert(result.buffer == (uint8_t *)bytes);
    } else {
        assert(result.buffer == NULL);
    }

    /* === Assertions for Step 4 (validity invariants) === */
    assert(aws_byte_buf_is_valid(&result));
}
