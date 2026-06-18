#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness() {
    const void *bytes;
    size_t len;

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    if (len > 0) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
    }

    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    assert(result.ptr == (uint8_t *)bytes);
    assert(result.len == len);
    assert(aws_byte_cursor_is_valid(&result));
}
