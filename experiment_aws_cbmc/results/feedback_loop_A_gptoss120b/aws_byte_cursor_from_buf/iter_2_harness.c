#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

size_t nondet_size_t(void);

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_byte_buf buf;

    /* nondet capacity and length within bounds */
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* allocate underlying buffer if capacity > 0 */
    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    /* allocator can be NULL for this harness */
    buf.allocator = NULL;

    /* ensure the buffer satisfies its validity invariant */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state */
    struct aws_byte_buf old = buf;

    /* call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* postconditions on the returned cursor */
    assert(cur.ptr == old.buffer);
    assert(cur.len == old.len);

    /* the input buffer must remain unchanged */
    assert(buf.buffer == old.buffer);
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* invariants must still hold */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));
}
