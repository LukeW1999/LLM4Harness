#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_array_harness() {
    const void *bytes;
    size_t len;

    __CPROVER_assume(len == 0 || AWS_MEM_IS_READABLE(bytes, len));

    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    __CPROVER_assert(cur.ptr == (uint8_t *)bytes, "ptr matches bytes");
    __CPROVER_assert(cur.len == len, "len matches");
    __CPROVER_assert(aws_byte_cursor_is_valid(&cur), "cursor is valid");
}
