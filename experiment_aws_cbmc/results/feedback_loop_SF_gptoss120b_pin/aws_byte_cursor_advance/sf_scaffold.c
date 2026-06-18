#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* allocator for any allocations we need */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic capacity for the backing buffer */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);               /* bound for tractability */

    /* allocate backing memory */
    uint8_t *buffer = (uint8_t *)malloc(capacity);
    __CPROVER_assume(buffer != NULL || capacity == 0);

    /* construct a valid aws_byte_buf */
    struct aws_byte_buf buf;
    buf.buffer    = buffer;
    buf.len       = capacity;
    buf.capacity  = capacity;
    buf.allocator = allocator;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* create a cursor from the buffer */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&buf);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* nondeterministic length to advance */
    size_t len = nondet_size_t();

    /* snapshot pre‑call state */
    size_t   old_len = cursor.len;
    uint8_t *old_ptr = cursor.ptr;

    /* call the function under verification */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* ASSERT_POSTCONDITIONS_HERE */
}
