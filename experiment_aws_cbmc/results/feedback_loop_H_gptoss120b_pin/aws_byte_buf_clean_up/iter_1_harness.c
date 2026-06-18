#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Bound and initialize src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* Save a byte from src for immutability check */
    struct store_byte_from_buffer src_bytes;
    save_byte_from_array(src.buffer, src.len, &src_bytes);

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect src */
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);
        assert(dest.buffer != NULL);
        assert_bytes_match(dest.buffer, src.buffer, src.len);

        /* src must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        assert_byte_from_buffer_matches(src.buffer, &src_bytes);
    } else {
        /* on failure dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        assert_byte_from_buffer_matches(src.buffer, &src_bytes);
    }

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
