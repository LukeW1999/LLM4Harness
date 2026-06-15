#include <stddef.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>

/* Stub for aws_mem_acquire: nondeterministically returns either a valid block of given size or NULL */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    if (size == 0) {
        return NULL;
    }
    void *ptr = malloc(size);
    if (ptr) {
        /* make the memory writable: fill with nondet values */
        for (size_t i = 0; i < size; i++) {
            ((uint8_t *)ptr)[i] = nondet_uint8_t();
        }
    }
    return ptr;
}

void aws_byte_buf_init_copy_harness() {
    /* Data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;

    /* Allocator: non-deterministic but non-null (precondition) */
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    __CPROVER_assume(allocator != NULL);

    /* Bound and prepare src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a copy of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */

    /* 1. src is never modified */
    assert(aws_byte_buf_is_valid(&src));
    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    if (src.len > 0 && src.buffer) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    if (result == AWS_OP_SUCCESS) {
        /* On success:

           - dest is always valid
           - dest->allocator == allocator
           - dest->len == src->len
           - dest->capacity == src->capacity   ( per implementation )
           - If src->buffer was NULL, dest->buffer == NULL, else dest->buffer != NULL
             and its first src->len bytes match src->buffer */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure (allocation failure), dest is completely zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* dest need not be valid because it is zeroed; a zeroed struct is valid if capacity==0 */
        assert(aws_byte_buf_is_valid(&dest));
    }
}
