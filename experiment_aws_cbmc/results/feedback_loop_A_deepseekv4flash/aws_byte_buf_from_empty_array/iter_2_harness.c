#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

/* Stub for aws_mem_acquire: non-deterministically returns NULL or valid memory */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    if (nondet_bool()) {
        return size == 0 ? NULL : malloc(size);
    } else {
        return NULL;
    }
}

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare parameters */
    struct aws_byte_buf *dest = malloc(sizeof(*dest));
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    struct aws_byte_buf *src = malloc(sizeof(*src));

    /* 2. Assumptions: all pointers non-null, src valid, allocator valid */
    __CPROVER_assume(dest != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(src != NULL);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old state of src */
    struct aws_byte_buf old_src = *src;

    /* 3. Call function */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest is valid */
        assert(aws_byte_buf_is_valid(dest));
        /* allocator is set */
        assert(dest->allocator == allocator);
        /* dest->len equals src->len, dest->capacity equals src->capacity */
        assert(dest->len == old_src.len);
        assert(dest->capacity == old_src.capacity);
        /* If src->buffer was non-null, dest->buffer is newly allocated and contents match */
        if (old_src.buffer != NULL) {
            assert(dest->buffer != NULL);
            assert_bytes_match(dest->buffer, old_src.buffer, old_src.len);
            assert(AWS_MEM_IS_WRITABLE(dest->buffer, old_src.capacity));
        } else {
            /* src->buffer was NULL, dest should have NULL buffer */
            assert(dest->buffer == NULL);
        }
    } else {
        /* On failure, dest is zeroed (buffer NULL, len=0, capacity=0, allocator=NULL) */
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
        assert(aws_byte_buf_is_valid(dest));
    }

    /* 5. src must remain unchanged */
    assert(src->buffer == old_src.buffer);
    assert(src->len == old_src.len);
    assert(src->capacity == old_src.capacity);
    assert(src->allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(src));

    /* 6. Free allocated memory if any (safe for CBMC as it tracks allocations) */
    if (dest->buffer) free(dest->buffer);
    if (src->buffer) free(src->buffer);
    free(dest);
    free(src);
    free(allocator);
}
