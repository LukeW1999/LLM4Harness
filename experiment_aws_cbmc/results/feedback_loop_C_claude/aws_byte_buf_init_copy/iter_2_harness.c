#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness(void) {
    /* Allocate and set up src */
    struct aws_byte_buf src_buf;
    struct aws_byte_buf *src = &src_buf;

    /* Nondeterministically decide if buffer is NULL or allocated */
    if (nondet_bool()) {
        src->buffer = NULL;
        src->len = 0;
        src->capacity = 0;
        src->allocator = NULL;
    } else {
        /* Bound the capacity to keep verification tractable */
        size_t cap;
        __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
        src->capacity = cap;
        size_t len;
        __CPROVER_assume(len <= src->capacity);
        src->len = len;
        src->allocator = aws_default_allocator();
        src->buffer = malloc(src->capacity > 0 ? src->capacity : 1);
        __CPROVER_assume(src->buffer != NULL);
    }

    /* Precondition: src must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old src state to verify frame conditions */
    struct aws_byte_buf old_src = *src;

    /* Set up dest (uninitialized, as it's an output parameter) */
    struct aws_byte_buf dest_buf;
    struct aws_byte_buf *dest = &dest_buf;

    /* Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* === Verify frame conditions for src (must be unchanged) === */
    assert(src->len == old_src.len);
    assert(src->buffer == old_src.buffer);
    assert(src->capacity == old_src.capacity);
    assert(src->allocator == old_src.allocator);

    /* === Verify postconditions based on result === */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid after successful call */
        assert(aws_byte_buf_is_valid(dest));

        /* allocator must be set correctly */
        assert(dest->allocator == allocator);

        if (old_src.buffer == NULL) {
            /* Case A: src->buffer was NULL */
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            /* Case B: src->buffer was not NULL */
            assert(dest->buffer != NULL);
            assert(dest->len == old_src.len);
            assert(dest->capacity == old_src.capacity);
            /* Verify the copy: contents match */
            if (old_src.len > 0) {
                assert_bytes_match(dest->buffer, old_src.buffer, old_src.len);
            }
        }
    } else {
        /* Failure path: dest should be zeroed */
        assert(result == AWS_OP_ERR);
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }
}
