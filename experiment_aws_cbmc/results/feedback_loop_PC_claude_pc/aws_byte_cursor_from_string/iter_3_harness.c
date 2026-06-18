#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Allocate a valid aws_string */
    const struct aws_string *src = nondet_const_aws_string_ptr();
    __CPROVER_assume(aws_string_is_valid(src));

    /* Snapshot src fields before the call (for frame conditions) */
    size_t old_len = src->len;
    const uint8_t *old_bytes = src->bytes;

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* --- Postconditions --- */

    /* Non-NULL input: cursor must point to string bytes with correct length */
    assert(result.ptr == (uint8_t *)src->bytes);
    assert(result.len == src->len);

    /* cursor length matches string length */
    assert(result.len == old_len);

    /* if len > 0, ptr must not be NULL */
    if (result.len > 0) {
        assert(result.ptr != NULL);
    }

    /* INVARIANTS: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
}
