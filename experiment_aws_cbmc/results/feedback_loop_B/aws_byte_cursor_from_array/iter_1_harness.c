#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_byte_cursor_from_array_harness() {
    /* 1. Declare and bound data structures */
    const void *bytes = (const void *)nondet_size_t();
    size_t len = nondet_size_t();

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    /* No need to save state as we are not modifying any structure here */

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since this function always succeeds and there's no failure path, we only need to check success conditions */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* 5. Assert fields that must NOT change regardless of result */
    /* No fields to assert as no structure is modified */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));
}
