// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_from_empty_array always returns a struct (not an int), so there's no error path.
// The function always succeeds and returns a struct aws_byte_buf.
// Given: const void *bytes, size_t capacity
//   - buf.buffer: SET to (uint8_t *)bytes if capacity > 0, else NULL
//   - buf.len: SET to 0
//   - buf.capacity: SET to capacity
//   - buf.allocator: SET to NULL
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns a value.
//
// === STEP 3: FRAME CONDITIONS ===
// Return value (struct aws_byte_buf):
//   - buffer: CHANGED - set to (capacity > 0) ? (uint8_t *)bytes : NULL
//   - len: CHANGED - always set to 0
//   - capacity: CHANGED - set to capacity parameter
//   - allocator: CHANGED - set to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&result): YES (must hold after call)
//     The validity check requires:
//       (capacity == 0 && len == 0 && buffer == NULL) OR
//       (capacity > 0 && len <= capacity && buffer is writable for capacity bytes)
//     Since len=0 and buffer=bytes (writable), this holds when bytes is valid.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_buf_from_empty_array_harness(void) {
    // Non-deterministic inputs
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    // Allocate bytes array - must be writable for capacity bytes
    uint8_t *bytes = NULL;
    if (capacity > 0) {
        bytes = malloc(capacity);
        __CPROVER_assume(bytes != NULL);
    }

    // Call the function under test
    struct aws_byte_buf result = aws_byte_buf_from_empty_array(bytes, capacity);

    // === STEP 1 assertions: verify return value fields ===
    // buf.buffer: set to (uint8_t *)bytes if capacity > 0, else NULL
    if (capacity > 0) {
        assert(result.buffer == (uint8_t *)bytes);
    } else {
        assert(result.buffer == NULL);
    }

    // buf.len: always set to 0
    assert(result.len == 0);

    // buf.capacity: set to capacity parameter
    assert(result.capacity == capacity);

    // buf.allocator: set to NULL
    assert(result.allocator == NULL);

    // === STEP 4: validity invariant ===
    assert(aws_byte_buf_is_valid(&result));
}
