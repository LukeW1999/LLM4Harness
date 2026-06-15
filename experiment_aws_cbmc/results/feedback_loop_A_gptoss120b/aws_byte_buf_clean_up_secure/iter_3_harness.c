#include <assert.h>
#include <stdlib.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/save_byte_from_array.h"

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = nondet_pointer();
    __CPROVER_assume(alloc != NULL);
    buf.allocator = alloc;

    /* nondet capacity within a reasonable bound */
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.len <= buf.capacity);

    /* allocate buffer if capacity > 0; malloc may succeed or fail */
    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        /* malloc may return NULL, which is allowed */
    } else {
        buf.buffer = NULL;
    }

    /* ensure the byte buffer is in a valid state before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state for immutability checks */
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer old_byte;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &old_byte);
    }

    /* call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* post‑condition checks */
    assert(buf.allocator == old.allocator);   /* allocator unchanged */
    assert(buf.buffer == NULL);               /* buffer deallocated */
    assert(buf.len == 0);                     /* length reset */
    assert(buf.capacity == 0);                /* capacity reset */
    assert(aws_byte_buf_is_valid(&buf));      /* still a valid byte buffer */
}
