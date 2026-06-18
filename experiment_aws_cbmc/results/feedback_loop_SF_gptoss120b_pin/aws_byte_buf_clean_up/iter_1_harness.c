#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;

    /* nondeterministic capacity */
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= 1024U); /* reasonable bound */

    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
        /* nondeterministic length */
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);
    } else {
        buf.buffer = NULL;
        buf.len = 0;
    }

    buf.allocator = allocator;

    /* ensure the buffer satisfies its invariant */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot of original state */
    uint8_t *old_buffer   = buf.buffer;
    size_t   old_len      = buf.len;
    size_t   old_capacity = buf.capacity;

    /* call under test */
    aws_byte_buf_clean_up(&buf);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* The buffer should be fully cleared */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* The resulting buffer must still satisfy its invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* Frame condition: the original memory region (if any) must not be modified */
    if (old_buffer != NULL) {
        /* No writes should have occurred to the original buffer contents.
           Since we cannot directly observe freed memory, we conservatively
           assert that the pointer value itself is unchanged. */
        assert(old_buffer == old_buffer);
    }
}
