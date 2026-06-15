#include <aws/common/byte_buf.h>
#include <aws/common/memory.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/assertions.h"

/* Maximum size for the buffer in the harness */
#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_from_array_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf buf;
    const uint8_t *bytes;
    size_t len = nondet_size_t();

    /* 2. Constrain length */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 3. Allocate / set pointer for the source array */
    bytes = nondet_uint8_t() ? (uint8_t *)0x1 : (uint8_t *)0x1; /* non‑NULL when len > 0 */
    __CPROVER_assume((len == 0) ? (bytes == NULL) : (bytes != NULL));

    /* 4. Make the source array readable for len bytes */
    __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));

    /* 5. Preserve a copy of the source data for later comparison */
    uint8_t *src_copy = NULL;
    if (len > 0) {
        src_copy = malloc(len);
        __CPROVER_assume(src_copy != NULL);
        memcpy(src_copy, bytes, len);
    }

    /* 6. Call function under test */
    int result = aws_byte_buf_from_array(&buf, bytes, len);

    /* 7. Post‑conditions */
    /* The function should always succeed */
    assert(result == AWS_OP_SUCCESS);

    /* The resulting byte buffer must be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Buffer fields must reflect the input array */
    if (len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer != NULL);
        assert(buf.buffer == bytes);
    }
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* The buffer must contain a copy of the original data (i.e., point to the same memory) */
    if (len > 0) {
        assert_bytes_match(buf.buffer, bytes, len);
        /* Ensure the source array was not modified */
        assert_bytes_match(bytes, src_copy, len);
    }

    /* Clean up */
    free(src_copy);
}
