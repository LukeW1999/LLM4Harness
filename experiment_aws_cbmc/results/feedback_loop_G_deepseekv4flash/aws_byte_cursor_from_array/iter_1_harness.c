#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_array_harness() {
    size_t len;
    const void *bytes;

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    if (len > 0) {
        __CPROVER_assume(bytes != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
    } else {
        /* len == 0 allows bytes to be NULL or any valid pointer */
        /* No additional assumption needed; nondet bytes is fine */
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* Postcondition: cursor correctly reflects input */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* The resulting cursor should be valid if the input was valid */
    /* (We assumed inputs that produce a valid cursor) */
    assert(aws_byte_cursor_is_valid(&cur));
}
