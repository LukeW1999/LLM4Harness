#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_from_array_harness(void) {
    struct aws_byte_buf buf;
    uint8_t *bytes;
    size_t len;

    /* nondet length bounded by MAX_BUFFER_SIZE */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate source array if length is non‑zero */
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        bytes = NULL;
    }

    /* Save old state of buf (it will be overwritten) */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_from_array(&buf, bytes, len);

    /* Post‑conditions */
    assert(buf.allocator == NULL);
    assert(buf.buffer == bytes);
    assert(buf.len == len);
    assert(buf.capacity == len);

    if (bytes == NULL) {
        assert(len == 0);
    }

    /* Invariants */
    assert(aws_byte_buf_is_valid(&buf));
}
