#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Declare inputs */
    size_t len;
    void *bytes;

    /* Non-deterministic length, bounded to keep state space manageable */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Non-deterministically allocate bytes array:
       either NULL (len == 0 case) or a valid pointer */
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        /* bytes can be anything when len == 0 */
        bytes = nondet_bool() ? NULL : malloc(1);
    }

    /* 2. Call function under test — returns struct by value */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* 3. Assert postconditions:
       The function simply assigns ptr = bytes and len = len */

    /* Changed fields: ptr and len are set from inputs */
    assert(result.ptr == (uint8_t *)bytes);
    assert(result.len == len);

    /* 4. Validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
