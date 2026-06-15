#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;

    /* bound and allocate the buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state */
    struct aws_byte_buf old = buf;
    uint8_t *old_bytes = NULL;
    if (buf.buffer != NULL && buf.capacity > 0) {
        old_bytes = malloc(buf.capacity);
        __CPROVER_assume(old_bytes);
        memcpy(old_bytes, buf.buffer, buf.capacity);
    }

    bool zero = nondet_bool();

    /* call function under test */
    aws_byte_buf_reset(&buf, zero);

    /* post‑conditions */
    assert(buf.len == 0);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == old.buffer);

    if (zero && buf.buffer != NULL) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        if (old_bytes != NULL) {
            for (size_t i = 0; i < buf.capacity; ++i) {
                assert(buf.buffer[i] == old_bytes[i]);
            }
        }
    }

    /* invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));

    free(old_bytes);
}
