#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Non-deterministic parameters */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    struct aws_byte_cursor src;

    /* Preconditions */
    __CPROVER_assume(allocator != NULL);                    // from AWS_PRECONDITION(allocator)
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);   // allocates src.ptr with nmemb = src.len
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));       // validates the whole structure

    /* Save old state of src for postcondition checks (dest is output; no need to save old) */
    struct aws_byte_cursor old_src = src;

    /* Call the function */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest is a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->len and dest->capacity equal src.len */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        /* dest->allocator equals the provided allocator */
        assert(dest.allocator == allocator);

        /* dest->buffer: if src.len > 0, it is a newly allocated buffer containing a copy of src data */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        /* src is unchanged (passed by value, but confirm its fields) */
        assert(src.ptr == old_src.ptr);
        assert(src.len == old_src.len);
    } else {
        /* On failure, dest should be zero-initialized */
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        /* src is unchanged */
        assert(src.ptr == old_src.ptr);
        assert(src.len == old_src.len);
    }

    /* Validity invariants for src (dest validity already checked) */
    assert(aws_byte_cursor_is_valid(&src));
}
