#include <assert.h>
#include <aws/common/byte_buf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

int main(void) {
    struct aws_byte_buf buf;

    /* nondeterministic length, bounded */
    size_t len = (size_t)__CPROVER_nondet_uint();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* source array – may be NULL when len == 0 */
    uint8_t *bytes = NULL;
    if (len == 0) {
        int make_null = __CPROVER_nondet_int();
        if (make_null) {
            bytes = NULL;
        } else {
            bytes = malloc(1);
            __CPROVER_assume(bytes != NULL);
        }
    } else {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    uint8_t *expected = bytes;

    /* call the function under test */
    aws_byte_buf_from_array(&buf, bytes, len);

    /* post‑condition checks */
    assert(buf.buffer == expected);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    /* source pointer must remain unchanged */
    assert(bytes == expected);

    return 0;
}
