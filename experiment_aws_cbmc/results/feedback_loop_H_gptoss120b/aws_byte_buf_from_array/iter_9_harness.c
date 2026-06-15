#include <aws/common/byte_buf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* Maximum length for the buffer to keep the state space bounded */
#define MAX_BUFFER_SIZE 1024

int main(void) {
    struct aws_byte_buf buf;

    /* nondeterministic length */
    size_t len = __CPROVER_nondet_uint();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* source array – may be NULL when len == 0 */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        /* allow both NULL and a non‑NULL pointer for zero length */
        int make_null = __CPROVER_nondet_int();
        if (make_null) {
            bytes = NULL;
        } else {
            bytes = malloc(1);
            __CPROVER_assume(bytes != NULL);
        }
    }
    uint8_t *old_bytes = bytes;

    /* function under test */
    aws_byte_buf_from_array(&buf, bytes, len);

    /* post‑condition checks */
    __CPROVER_assert(buf.buffer == old_bytes,
                     "buffer pointer must be the original array");
    __CPROVER_assert(buf.len == len,
                     "length must match the supplied length");
    __CPROVER_assert(buf.capacity == len,
                     "capacity must match the supplied length");
    __CPROVER_assert(buf.allocator == NULL,
                     "allocator must be NULL for from_array");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf),
                     "resulting byte buffer must be valid");

    /* source pointer must remain unchanged */
    __CPROVER_assert(bytes == old_bytes,
                     "source pointer must not be modified");

    return 0;
}
