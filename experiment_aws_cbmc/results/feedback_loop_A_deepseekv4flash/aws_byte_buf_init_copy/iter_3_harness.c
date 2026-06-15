#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

/* Stub for aws_mem_acquire: models allocation success/failure */
uint8_t *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    if (nondet_bool()) {
        return (uint8_t *)malloc(size);
    } else {
        return NULL;
    }
}

void aws_byte_buf_init_copy_harness() {
    /* nondet inputs */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));

    /* bound src to limit state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    /* ensure src is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* save src state for later comparison */
    struct aws_byte_buf old_src = src;

    /* call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* postconditions */

    /* src must be unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (old_src.buffer != NULL) {
        assert(src.buffer == old_src.buffer);
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    } else {
        assert(src.buffer == NULL);
    }
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        /* on success, dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator is set to the passed allocator */
        assert(dest.allocator == allocator);

        if (old_src.buffer == NULL) {
            /* src had no buffer: dest is zeroed */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
        } else {
            /* src had a buffer: dest gets a copy */
            assert(dest.len == old_src.len);
            assert(dest.capacity == old_src.capacity);
            assert(dest.buffer != NULL);
            /* the contents of dest->buffer must match src->buffer */
            assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
        }
    } else {
        /* on failure, dest is zeroed (AWS_ZERO_STRUCT) */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        /* note: allocator is also zeroed, becomes NULL */
        assert(dest.allocator == NULL);
    }
}
