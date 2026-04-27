#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Declare inputs */
    size_t len = nondet_size_t();
    void *bytes;

    /* If len is 0, bytes may be NULL per the API contract */
    if (len > 0) {
        /* Allocate a non-deterministic array of bytes */
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        /* len == 0: bytes may be NULL */
        bytes = nondet_bool() ? NULL : malloc(1);
    }

    /* 2. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* 3. Assert postconditions */

    /* Changed fields: ptr and len are set from the arguments */
    assert(result.ptr == (uint8_t *)bytes);
    assert(result.len == len);

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
