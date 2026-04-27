// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_from_array always "succeeds" (it's not an int-returning function, it returns a struct aws_byte_buf)
// The function creates a new aws_byte_buf from a given bytes pointer and len:
//   - result.buffer = (len > 0) ? (uint8_t *)bytes : NULL
//   - result.len = len
//   - result.capacity = len
//   - result.allocator = NULL
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns a valid aws_byte_buf
//
// === STEP 3: FRAME CONDITIONS ===
// result (struct aws_byte_buf, returned by value):
//   - result.buffer: SET to bytes if len > 0, else NULL
//   - result.len: SET to len
//   - result.capacity: SET to len
//   - result.allocator: SET to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&result): YES (postcondition in implementation)
//   The validity check:
//     (capacity == 0 && len == 0 && buffer == NULL) OR
//     (capacity > 0 && len <= capacity && buffer is writable for capacity bytes)
//   Since capacity = len:
//     if len == 0: capacity=0, len=0, buffer=NULL -> valid
//     if len > 0: capacity=len>0, len<=capacity (equal), buffer=bytes (writable) -> valid

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_buf_from_array_harness(void) {
    // Create nondeterministic inputs
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    // Call the function under test
    struct aws_byte_buf result = aws_byte_buf_from_array(bytes, len);

    // === STEP 1 assertions: check all fields of result ===
    // buffer: should be bytes if len > 0, else NULL
    if (len > 0) {
        assert(result.buffer == (uint8_t *)bytes);
    } else {
        assert(result.buffer == NULL);
    }

    // len field should equal the input len
    assert(result.len == len);

    // capacity should equal the input len
    assert(result.capacity == len);

    // allocator should be NULL
    assert(result.allocator == NULL);

    // === STEP 4: validity invariant ===
    assert(aws_byte_buf_is_valid(&result));
}
