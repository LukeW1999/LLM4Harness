#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness(void) {
    /* nondeterministic byte buffer */
    struct aws_byte_buf buf;
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();

    /* ensure length does not exceed capacity */
    __CPROVER_assume(buf.len <= buf.capacity);

    /* allocate underlying storage if capacity > 0 */
    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    /* allocator can be NULL or the default allocator */
    buf.allocator = aws_default_allocator();

    /* assume the buffer satisfies the library's validity predicate */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot of the buffer to check the frame condition later */
    struct aws_byte_buf old_buf = buf;

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* post‑condition checks */
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);
    assert(aws_byte_cursor_is_valid(&cur));

    /* frame condition: the input buffer must be unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    return 0;
}
