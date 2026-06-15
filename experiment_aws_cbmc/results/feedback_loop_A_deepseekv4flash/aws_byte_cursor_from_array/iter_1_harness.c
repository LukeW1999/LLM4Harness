#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_array_harness() {
    /* Non-deterministic inputs */
    const void *bytes;
    size_t len;

    /* Precondition: if len > 0, bytes must be readable */
    if (len > 0) {
        __CPROVER_assume(bytes != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
    } else {
        /* bytes may be NULL or any pointer when len == 0 */
        __CPROVER_assume(nondet_bool() ? bytes == NULL : bytes == malloc(1));
    }

    /* Call function */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* Postcondition: returned cursor matches inputs */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* Additional validity: if len > 0, ptr cannot be NULL */
    if (len > 0) {
        assert(cur.ptr != NULL);
    }

    /* No other side effects: the inputs are unchanged (by value) */
    /* The function does not modify any global state, so no further assertions. */
}
