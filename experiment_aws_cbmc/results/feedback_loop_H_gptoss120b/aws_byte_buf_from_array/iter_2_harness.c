#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>

/* Define a reasonable maximum for the buffer size used in the proof */
#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_from_array_harness(void) {
    struct aws_byte_buf buf;

    /* Assume the destination buffer structure is bounded */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Nondeterministic length bounded by MAX_BUFFER_SIZE */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a nondeterministic byte array when len > 0 */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* Preserve the original pointer for later checks */
    uint8_t *old_bytes = bytes;

    /* Call the function under test */
    aws_byte_buf_from_array(&buf, bytes, len);

    /* Post‑condition checks */
    assert(buf.buffer == old_bytes);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    /* The source array pointer must remain unchanged */
    assert(bytes == old_bytes);
}
