#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_array_harness() {
    /* Non-deterministic inputs */
    const void *bytes;
    size_t len;

    /* Precondition: if len > 0, then bytes must point to readable memory */
    __CPROVER_assume(len == 0 || AWS_MEM_IS_READABLE(bytes, len));

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* Postconditions */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* The returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cur));
}
