#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness() {
    /* 1. Declare and bound data structures */
    const void *bytes = nondet_uint8_t_ptr();
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    const void *old_bytes = bytes;
    size_t old_len = len;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(bytes == old_bytes);
    assert(len == old_len);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));
}
