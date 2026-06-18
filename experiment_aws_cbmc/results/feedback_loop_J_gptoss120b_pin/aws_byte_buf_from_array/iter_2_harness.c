#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf buf;
    uint8_t *bytes;
    size_t len;

    /* Choose a nondeterministic length bounded by MAX_BUFFER_SIZE */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a buffer of the chosen length */
    bytes = (uint8_t *)malloc(len);
    if (len > 0) {
        __CPROVER_assume(bytes != NULL);
    }

    /* Call the function under test */
    int result = aws_byte_buf_from_array(&buf, bytes, len);

    /* The function should always succeed */
    assert(result == AWS_OP_SUCCESS);

    /* Verify that the byte buffer now points to the provided array */
    assert(buf.buffer == bytes);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* The resulting buffer must satisfy its invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
