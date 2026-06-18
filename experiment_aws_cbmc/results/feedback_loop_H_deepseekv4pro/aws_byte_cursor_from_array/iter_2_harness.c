#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_cursor_from_array_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = (uint8_t *)malloc(len);
        __CPROVER_assume(bytes != NULL);
        /* fill with defined values to avoid uninitialized reads */
        for (size_t i = 0; i < len; i++) {
            bytes[i] = (uint8_t)(i % 256);
        }
    }

    /* save a random byte from the input to verify immutability */
    struct store_byte_from_buffer storage;
    if (len > 0) {
        save_byte_from_array(bytes, len, &storage);
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array((void *)bytes, len);

    /* postcondition: the fields are set to the inputs */
    assert(cur.ptr == bytes);
    assert(cur.len == len);

    /* the produced cursor should satisfy the validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));

    /* the original array must not have been modified (read-only operation) */
    if (len > 0) {
        assert_byte_from_buffer_matches(bytes, &storage);
        free(bytes);
    }
}
