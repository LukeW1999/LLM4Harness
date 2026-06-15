#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

int main(void) {
    struct aws_byte_buf buf;

    /* nondeterministic length, zero and non‑zero cases reachable */
    size_t len = __CPROVER_nondet_uint();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* source array – allocate at least 1 byte so the pointer is never NULL */
    uint8_t *bytes = malloc(len ? len : 1);
    __CPROVER_assume(bytes != NULL);
    uint8_t *old_bytes = bytes;

    /* function under test */
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
