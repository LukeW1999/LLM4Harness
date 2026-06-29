#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound buffer sizes for CBMC tractability */
#define MAX_BUFFER_SIZE 8

int aws_byte_buf_init_copy(
    struct aws_byte_buf *dest,
    struct aws_allocator *allocator,
    const struct aws_byte_buf *src);

void aws_byte_buf_init_copy_harness(void) {
    /* Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Set up src with nondet fields, bounded for CBMC */
    struct aws_byte_buf src;
    size_t src_capacity;
    size_t src_len;

    __CPROVER_assume(src_capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(src_len <= src_capacity);

    src.capacity = src_capacity;
    src.len = src_len;
    src.allocator = allocator; /* src allocator can be anything valid */

    /* Nondet: src->buffer is either NULL or a valid allocation */
    bool src_has_buffer;
    if (src_has_buffer && src_capacity > 0) {
        src.buffer = malloc(src_capacity);
        __CPROVER_assume(src.buffer != NULL);
    } else {
        src.buffer = NULL;
        /* If buffer is NULL, len and capacity must be 0 for aws_byte_buf_is_valid */
        src.len = 0;
        src.capacity = 0;
    }

    /* Precondition: src must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Set up dest as uninitialized writable memory */
    struct aws_byte_buf dest;

    /* Save src state before call to verify frame condition */
    struct aws_byte_buf src_before = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (src.buffer == NULL) {
        /* src->buffer was NULL branch */
        assert(result == AWS_OP_SUCCESS);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
    } else {
        /* src->buffer was non-NULL branch */
        if (result == AWS_OP_SUCCESS) {
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            assert(aws_byte_buf_is_valid(&dest));
        } else {
            /* Allocation failed */
            assert(result == AWS_OP_ERR);
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == NULL);
        }
    }

    /* Frame condition: src is not modified */
    assert(src.buffer == src_before.buffer);
    assert(src.len == src_before.len);
    assert(src.capacity == src_before.capacity);
    assert(src.allocator == src_before.allocator);

    /* Cleanup to avoid memory leaks in harness */
    if (result == AWS_OP_SUCCESS && dest.buffer != NULL) {
        aws_byte_buf_clean_up(&dest);
    }
}
