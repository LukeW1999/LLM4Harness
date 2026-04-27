#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_array_harness() {
    /* parameters */
    size_t length;
    const void *bytes;

    /* assumptions */
    __CPROVER_assume(length <= MAX_BUFFER_SIZE);
    ASSUME_VALID_MEMORY_COUNT(bytes, length);

    /* operation under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, length);

    /* assertions */
    assert(aws_byte_cursor_is_valid(&cur));
    assert(cur.len == length);
    assert(cur.ptr == (uint8_t *)bytes);
}
