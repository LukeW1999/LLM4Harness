#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

int main(void) {
    struct aws_byte_buf buf;

    /* nondeterministic length, both zero and non‑zero cases reachable */
    unsigned char choose_len = __CPROVER_nondet_uchar();
    size_t len;
    if (choose_len) {
        len = 0;
    } else {
        len = __CPROVER_nondet_uint();
        __CPROVER_assume(len > 0 && len <= MAX_BUFFER_SIZE);
    }

    /* source array */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }
    uint8_t *old_bytes = bytes;

    /* function under test */
    aws_byte_buf_from_array(&buf, bytes, len);

    /* post‑condition checks */
    __CPROVER_assert(buf.buffer == old_bytes, "buf.buffer must equal source pointer");
    __CPROVER_assert(buf.len == len, "buf.len must equal length");
    __CPROVER_assert(buf.capacity == len, "buf.capacity must equal length");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator must be NULL");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf must be valid");

    /* source pointer must remain unchanged */
    __CPROVER_assert(bytes == old_bytes, "source pointer unchanged");

    return 0;
}
