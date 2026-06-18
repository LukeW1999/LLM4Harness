#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_from_array_harness() {
    size_t len;
    void *bytes;

    /* bound the length to reduce state space */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    if (len > 0) {
        __CPROVER_assume(bytes != NULL);
        AWS_MEM_IS_READABLE(bytes, len);
    }

    /* save a random byte from the input to verify immutability */
    struct store_byte_from_buffer storage;
    if (len > 0) {
        save_byte_from_array((uint8_t *)bytes, len, &storage);
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* postcondition: the fields are set to the inputs */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* the produced cursor should satisfy the validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));

    /* the original array must not have been modified (read-only operation) */
    if (len > 0) {
        assert_byte_from_buffer_matches((uint8_t *)bytes, &storage);
    }
}
