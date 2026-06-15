#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_cat_harness() {
    /* Non-deterministic data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src1;
    struct aws_byte_buf src2;

    /* Bounding and allocation for dest and sources */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* Save old states for dest and sources */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    /* Save bytes from source buffers for later comparison */
    struct store_byte_from_buffer stored_bytes_src1;
    if (src1.buffer != NULL && src1.len > 0) {
        save_byte_from_array(src1.buffer, src1.len, &stored_bytes_src1);
    }
    struct store_byte_from_buffer stored_bytes_src2;
    if (src2.buffer != NULL && src2.len > 0) {
        save_byte_from_array(src2.buffer, src2.len, &stored_bytes_src2);
    }

    /* Number of arguments: fixed to 2 for this harness */
    size_t number_of_args = 2;

    /* Invoke the function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args, &src1, &src2);

    /* Postconditions */
    /* 1. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));

    /* 2. Fields that must NOT change on either path */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);
    /* Source buffers must be unchanged */
    assert(src1.allocator == old_src1.allocator);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.len == old_src1.len);
    assert(src2.allocator == old_src2.allocator);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.buffer == old_src2.buffer);
    assert(src2.len == old_src2.len);
    /* Source buffer contents unchanged */
    if (old_src1.buffer != NULL && old_src1.len > 0) {
        assert_byte_from_buffer_matches(src1.buffer, &stored_bytes_src1);
    }
    if (old_src2.buffer != NULL && old_src2.len > 0) {
        assert_byte_from_buffer_matches(src2.buffer, &stored_bytes_src2);
    }

    /* 3. Changed fields depending on result */
    if (result == AWS_OP_SUCCESS) {
        /* All appends succeeded; dest->len increased by sum of source lengths */
        size_t total_src_len = old_src1.len + old_src2.len;
        size_t new_len = old_dest.len + total_src_len;
        if (new_len > old_dest.capacity) {
            /* If capacity was insufficient, append would fail, so success implies capacity sufficient */
            assert(new_len <= old_dest.capacity);
        }
        assert(dest.len == new_len);
    } else {
        /* On failure, dest->len must be >= old_dest.len (some appends may have succeeded) */
        assert(dest.len >= old_dest.len);
        /* dest->len cannot exceed old_dest.len + total_src_len */
        assert(dest.len <= old_dest.len + old_src1.len + old_src2.len);
    }
}
