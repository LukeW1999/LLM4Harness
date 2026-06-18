#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness() {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* source buffers (maximum of 3 arguments) */
    struct aws_byte_buf src0, src1, src2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src0);
    __CPROVER_assume(aws_byte_buf_is_valid(&src0));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* nondeterministic number of arguments, bounded to 3 */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= 3);

    /* Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_dest_bytes;
    save_byte_from_array(dest.buffer, dest.capacity, &old_dest_bytes);

    struct aws_byte_buf old_src0 = src0;
    struct store_byte_from_buffer old_src0_bytes;
    save_byte_from_array(src0.buffer, src0.capacity, &old_src0_bytes);

    struct aws_byte_buf old_src1 = src1;
    struct store_byte_from_buffer old_src1_bytes;
    save_byte_from_array(src1.buffer, src1.capacity, &old_src1_bytes);

    struct aws_byte_buf old_src2 = src2;
    struct store_byte_from_buffer old_src2_bytes;
    save_byte_from_array(src2.buffer, src2.capacity, &old_src2_bytes);

    /* Call function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args,
                                 &src0,
                                 &src1,
                                 &src2);

    /* Compute total length of data that should have been appended on success */
    size_t total_append_len = 0;
    if (number_of_args > 0) {
        total_append_len += src0.len;
    }
    if (number_of_args > 1) {
        total_append_len += src1.len;
    }
    if (number_of_args > 2) {
        total_append_len += src2.len;
    }

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest length increased by total appended length */
        assert(dest.len == old_dest.len + total_append_len);

        /* capacity, allocator and buffer pointer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);

        /* original bytes before old_dest.len remain unchanged */
        assert_bytes_match(dest.buffer, old_dest.buffer, old_dest.len);

        /* appended region matches source buffers */
        size_t offset = old_dest.len;
        if (number_of_args > 0) {
            assert_bytes_match(dest.buffer + offset, src0.buffer, src0.len);
            offset += src0.len;
        }
        if (number_of_args > 1) {
            assert_bytes_match(dest.buffer + offset, src1.buffer, src1.len);
            offset += src1.len;
        }
        if (number_of_args > 2) {
            assert_bytes_match(dest.buffer + offset, src2.buffer, src2.len);
            offset += src2.len;
        }

        /* dest must have had enough capacity to hold the data */
        assert(dest.capacity >= old_dest.len + total_append_len);
    } else {
        /* On failure dest must be unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert_byte_from_buffer_matches(old_dest.buffer, &old_dest_bytes);
    }

    /* Source buffers must be unchanged regardless of result */
    assert(src0.len == old_src0.len);
    assert(src0.capacity == old_src0.capacity);
    assert(src0.allocator == old_src0.allocator);
    assert(src0.buffer == old_src0.buffer);
    assert_byte_from_buffer_matches(old_src0.buffer, &old_src0_bytes);

    assert(src1.len == old_src1.len);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);
    assert(src1.buffer == old_src1.buffer);
    assert_byte_from_buffer_matches(old_src1.buffer, &old_src1_bytes);

    assert(src2.len == old_src2.len);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.allocator == old_src2.allocator);
    assert(src2.buffer == old_src2.buffer);
    assert_byte_from_buffer_matches(old_src2.buffer, &old_src2_bytes);

    /* Validity invariants must hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src0));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
}
