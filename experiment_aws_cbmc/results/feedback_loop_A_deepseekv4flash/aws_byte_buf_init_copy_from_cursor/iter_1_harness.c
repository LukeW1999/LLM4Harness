#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/* Stub for aws_mem_acquire: non-deterministically returns NULL or a valid buffer */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    uint8_t *buffer = malloc(size);
    if (nondet_bool()) {
        free(buffer);
        return NULL;
    }
    return buffer;
}

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Build a non-deterministic valid cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save the original cursor content (if any) for later immutability check */
    struct store_byte_from_buffer src_bytes;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_bytes);
    }

    /* Non-deterministic allocator pointer (assumed to be valid) */
    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    /* Output buffer – uninitialized at start */
    struct aws_byte_buf dest;

    /* Call the function */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest is a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* As described in the Doxygen: dest->len = src.len, dest->capacity = src.len */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        /* Allocator is propagated */
        assert(dest.allocator == allocator);

        /* If source had data, the destination buffer is allocated and matches */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        /* The source cursor’s underlying memory is unchanged */
        if (src.len > 0) {
            assert_byte_from_buffer_matches(src.ptr, &src_bytes);
        }
    } else {
        /* Failure occurs only when src.len > 0 and allocation fails */
        /* The dest struct is zeroed (AWS_ZERO_STRUCT) before failure */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);

        /* Source is still valid and unchanged */
        assert(aws_byte_cursor_is_valid(&src));
        if (src.len > 0) {
            assert_byte_from_buffer_matches(src.ptr, &src_bytes);
        }
    }

    /* The source cursor always remains valid (passed by value, but underlying data intact) */
    assert(aws_byte_cursor_is_valid(&src));
}
