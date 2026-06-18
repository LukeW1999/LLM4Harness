#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Declare inputs non-deterministically */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* bytes pointer: either NULL (if len == 0) or a valid readable array */
    uint8_t *bytes;
    if (len == 0) {
        /* NULL is allowed when len is 0 */
        bytes = nondet_bool() ? NULL : malloc(len);
    } else {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* 2. Save input state before the call */
    const void *const old_bytes = bytes;
    const size_t old_len = len;

    /* 3. Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* 4. Assert postconditions */

    /* RETURN / OUTPUTS: the returned cursor must have ptr == bytes and len == len */
    assert(result.ptr == (uint8_t *)old_bytes);
    assert(result.len == old_len);

    /* FRAME: inputs must not have changed */
    assert(bytes == old_bytes);
    assert(len == old_len);

    /* INVARIANTS: the returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
}
