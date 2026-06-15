#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_byte_cursor_from_array_harness() {
    size_t len;
    const void *bytes = NULL;

    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    if (len > 0) {
        assert(cur.ptr != NULL);
    }
}
