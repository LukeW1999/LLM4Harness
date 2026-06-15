#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

/* Stub for aws_mem_acquire: returns either NULL or a fresh buffer of the requested size */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    if (nondet_bool()) {
        return NULL;
    }
    void *ptr = malloc(size);
    __CPROVER_assume(ptr != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(ptr, size));
    return ptr;
}

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* Preconditions: pointers non-null, src valid, allocator non-null */
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(src != NULL);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Bound src to avoid huge allocations */
    __CPROVER_assume(aws_byte_buf_is_bounded(src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(src);

    /* Save old state of src for immutability check */
    struct aws_byte_buf old_src = *src;
    struct store_byte_from_buffer old_src_buffer;
    if (src->buffer != NULL && src->len > 0) {
        save_byte_from_array(src->buffer, src->len, &old_src_buffer);
    }

    /* Call the function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest fields as per Doxygen */
        assert(dest->allocator == allocator);
        assert(dest->len == src->len);
        assert(dest->capacity == src->len);  /* Doxygen says capacity equals src len */
        if (src->len == 0) {
            assert(dest->buffer == NULL);
        } else {
            assert(dest->buffer != NULL);
            assert_bytes_match(dest->buffer, src->buffer, src->len);
        }
    } else {
        /* Failure: dest is zeroed */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }

    /* src must be unchanged */
    assert(src->allocator == old_src.allocator);
    assert(src->buffer == old_src.buffer);
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    if (src->buffer != NULL && src->len > 0) {
        assert_byte_from_buffer_matches(src->buffer, &old_src_buffer);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(dest));
    assert(aws_byte_buf_is_valid(src));
}
