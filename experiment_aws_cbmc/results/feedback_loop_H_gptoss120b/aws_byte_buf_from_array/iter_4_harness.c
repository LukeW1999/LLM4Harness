#include <aws/common/byte_buf.h>
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
    __CPROVER_assert(buf.buffer == old_bytes, "buf.buffer should equal original bytes pointer");
    __CPROVER_assert(buf.len == len, "buf.len should equal length");
    __CPROVER_assert(buf.capacity == len, "buf.capacity should equal length");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator should be NULL");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf should be valid");

    /* source pointer must remain unchanged */
    __CPROVER_assert(bytes == old_bytes, "source pointer must remain unchanged");

    return 0;
}
