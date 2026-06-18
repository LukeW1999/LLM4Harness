#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_from_array_harness() {
    /* 1. Declare inputs and bound them */
    struct aws_byte_buf dest;
    const uint8_t *bytes;
    size_t len;

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    if (len > 0) {
        uint8_t *buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        bytes = buf;
    } else {
        bytes = NULL;
    }

    /* Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    const uint8_t *old_bytes = bytes;

    struct store_byte_from_buffer bytes_storage;
    if (len > 0) {
        save_byte_from_array((uint8_t *)bytes, len, &bytes_storage);
    }

    /* 2. Call function under test */
    int result = aws_byte_buf_from_array(&dest, bytes, len);

    /* 3. Post‑condition checks */
    assert(result == AWS_OP_SUCCESS);
    assert(dest.buffer == bytes);
    assert(dest.len == len);
    assert(dest.capacity == len);
    assert(dest.allocator == NULL);

    if (len > 0) {
        assert(dest.buffer != NULL);
        assert_bytes_match(dest.buffer, bytes, len);
    } else {
        assert(dest.buffer == NULL);
    }

    /* 4. Ensure inputs unchanged */
    assert(bytes == old_bytes);
    if (len > 0) {
        assert_byte_from_buffer_matches((uint8_t *)bytes, &bytes_storage);
    }

    /* 5. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
}
