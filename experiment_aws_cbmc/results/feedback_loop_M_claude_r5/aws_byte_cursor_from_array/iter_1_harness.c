#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Declare and bound inputs */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a buffer of the given length (or NULL if len == 0) */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* 2. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* 3. Assert postconditions:
     *    - result.ptr == bytes (the pointer is set to the input array)
     *    - result.len == len   (the length is set to the input length)
     */
    assert(result.ptr == (uint8_t *)bytes);
    assert(result.len == len);

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
