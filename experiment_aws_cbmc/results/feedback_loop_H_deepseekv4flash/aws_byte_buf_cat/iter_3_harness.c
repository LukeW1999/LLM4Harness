#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src1;
    struct aws_byte_buf src2;

    // Initialize buffers with nondeterministic but valid values
    cbmc_populate_byte_buf(&dest);
    cbmc_populate_byte_buf(&src1);
    cbmc_populate_byte_buf(&src2);

    // Ensure validity
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    // Save old states
    struct aws_byte_buf old_dest = dest;
    size_t old_src1_len = src1.len;
    size_t old_src2_len = src2.len;

    // Call the function
    int result = aws_byte_buf_cat(&dest, 2, &src1, &src2);

    // Postconditions
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));

    // dest fields that must remain unchanged
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);

    // src buffers must be unchanged
    assert(src1.len == old_src1_len);
    assert(src2.len == old_src2_len);

    // Result is either success or failure
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    // For success, len must increase by total src lengths and not exceed capacity
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_dest.len + old_src1_len + old_src2_len);
        assert(dest.len <= old_dest.capacity);
    } else {
        // On failure, dest.len must be unchanged
        assert(dest.len == old_dest.len);
    }
}
