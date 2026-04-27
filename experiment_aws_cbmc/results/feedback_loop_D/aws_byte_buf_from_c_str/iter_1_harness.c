// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_from_c_str returns AWS_OP_SUCCESS (or the successful value):
//   - buf.len: CHANGES to strlen(c_str) if c_str is not NULL, otherwise 0
//   - buf.capacity: CHANGES to strlen(c_str) if c_str is not NULL, otherwise 0
//   - buf.buffer: CHANGES to (uint8_t *)c_str if c_str is not NULL, otherwise NULL
//   - buf.allocator: UNCHANGED (set to NULL)

// === STEP 2: FAILURE PATH ===
// There is no failure path for this function as it does not return an error code.
// The function always succeeds and returns a struct aws_byte_buf.

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - len: CHANGED on success
//     - capacity: CHANGED on success
//     - buffer: CHANGED on success
//     - allocator: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_from_c_str_harness() {
    const char *c_str = nondet_bool() ? "example" : NULL;
    struct aws_byte_buf old_buf;
    old_buf.len = nondet_size_t();
    old_buf.capacity = nondet_size_t();
    old_buf.buffer = (uint8_t *)nondet_voidp();
    old_buf.allocator = (struct aws_allocator *)nondet_voidp();

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    // Assertions based on frame conditions and validity invariants
    if (c_str != NULL) {
        assert(buf.len == strlen(c_str));
        assert(buf.capacity == strlen(c_str));
        assert(buf.buffer == (uint8_t *)c_str);
    } else {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    }
    assert(buf.allocator == NULL);

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));
}
