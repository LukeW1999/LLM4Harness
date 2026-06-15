#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

/* Stub for aws_mem_acquire: nondeterministically returns either a valid memory block or NULL */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    if (nondet_bool()) {
        void *mem = malloc(size);
        if (mem != NULL && size > 0) {
            /* Assume the memory is writable */
            __CPROVER_assume(AWS_MEM_IS_WRITABLE(mem, size));
        }
        return mem;
    } else {
        return NULL;
    }
}

#define MAX_BUFFER_SIZE 100

void aws_byte_buf_init_copy_harness() {
    /* Data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator;

    /* Allocator must be non-null */
    allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);

    /* Bounds and ensure src is valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save src state */
    struct aws_byte_buf old_src = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* dest should be zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            /* dest is a copy of src */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure, dest is zero-initialized */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must be unchanged */
    assert(aws_byte_buf_is_valid(&src));
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);

    /* dest must be valid */
    assert(aws_byte_buf_is_valid(&dest));
}
