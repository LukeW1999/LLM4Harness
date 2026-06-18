#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_from_array_harness() {
    /* Inputs */
    void *bytes;
    size_t len;

    /* Bounded nondeterminism */
    len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* bytes must be readable for len bytes */
    __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));

    /* Save a byte from the source array for immutability check (if len > 0) */
    struct store_byte_from_buffer old_byte;
    if (len > 0) {
        save_byte_from_array((uint8_t *)bytes, len, &old_byte);
    }

    /* Call function */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_array(bytes, len);

    /* Postconditions: cursor fields match inputs */
    assert(cursor.ptr == (uint8_t *)bytes);
    assert(cursor.len == len);

    /* Validity: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Immutability: the source array must be unchanged */
    if (len > 0) {
        assert_byte_from_buffer_matches((uint8_t *)bytes, &old_byte);
    }
}
