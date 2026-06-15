#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

/* Stub for aws_mem_acquire to model both success and failure */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    if (nondet_bool()) {
        if (size == 0) {
            return NULL;
        }
        void *ptr = malloc(size);
        __CPROVER_assume(ptr != NULL);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(ptr, size));
        return ptr;
    }
    return NULL;
}

void aws_byte_buf_init_copy_harness() {
    /* Nondeterministic inputs */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator;

    __CPROVER_assume(allocator != NULL);

    /* Constrain src to be bounded and valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old src state for immutability check */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.buffer != NULL) {
        save_byte_from_array(src.buffer, src.len, &src_storage);
    }

    /* Call the function under proof */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest takes ownership of a new allocation */
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* Failure: dest is zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src is const, so must remain unchanged */
    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    if (src.buffer != NULL) {
        assert_byte_from_buffer_matches(src.buffer, &src_storage);
    }
}
