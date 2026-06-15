#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_cursor_from_array_harness() {
    /* 1. Declare and bound data structures */
    uint8_t bytes[MAX_BUFFER_SIZE];
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Initialize bytes */
    for (size_t i = 0; i < len; i++) {
        bytes[i] = nondet_uint8_t();
    }

    /* 3. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(result.ptr == bytes);
    assert(result.len == len);

    /* 5. Assert fields that must NOT change regardless of result */
    /* No fields to assert as this function returns a new struct */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&result));
}

int main() {
    aws_byte_cursor_from_array_harness();
    return 0;
}
