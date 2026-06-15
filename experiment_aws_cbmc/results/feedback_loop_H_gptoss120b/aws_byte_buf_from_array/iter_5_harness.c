#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

int main(void) {
    struct aws_byte_buf buf;

    /* nondeterministic length bounded by MAX_BUFFER_SIZE */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* nondeterministic source array */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    uint8_t *old_bytes = bytes;

    /* call the function under test */
    aws_byte_buf_from_array(&buf, bytes, len);

    /* post‑condition checks */
    assert(buf.buffer == old_bytes);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    /* source pointer must remain unchanged */
    assert(bytes == old_bytes);

    return 0;
}
