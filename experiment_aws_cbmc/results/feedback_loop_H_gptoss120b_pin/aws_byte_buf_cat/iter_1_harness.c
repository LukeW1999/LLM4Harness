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

void aws_byte_buf_cat_harness(void) {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* source buffers (up to 2 arguments) */
    struct aws_byte_buf src0, src1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src0);
    __CPROVER_assume(aws_byte_buf_is_valid(&src0));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    /* nondet number of arguments (0..2) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= 2);

    /* save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src0 = src0;
    struct aws_byte_buf old_src1 = src1;

    struct store_byte_from_buffer src0_store;
    struct store_byte_from_buffer src1_store;
    struct store_byte_from_buffer dest_store;

    if (src0.buffer != NULL && src0.len > 0) {
        save_byte_from_array(src0.buffer, src0.len, &src0_store);
    }
    if (src1.buffer != NULL && src1.len > 0) {
        save_byte_from_array(src1.buffer, src1.len, &src1_store);
    }
    if (dest.buffer != NULL && dest.len > 0) {
        save_byte_from_array(dest.buffer, dest.len, &dest_store);
    }

    /* call function under test */
    int result;
    if (number_of_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (number_of_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, &src0);
    } else {
        result = aws_byte_buf_cat(&dest, 2, &src0, &src1);
    }

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest length increased by total appended length */
        size_t expected_len = old_dest.len;
        if (number_of_args >= 1) {
            expected_len += src0.len;
        }
        if (number_of_args == 2) {
            expected_len += src1.len;
        }
        assert(dest.len == expected_len);

        /* unchanged fields */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);

        /* content of appended region matches sources */
        size_t offset = old_dest.len;
        if (number_of_args >= 1 && src0.len > 0) {
            assert_bytes_match(src0.buffer, dest.buffer + offset, src0.len);
            offset += src0.len;
        }
        if (number_of_args == 2 && src1.len > 0) {
            assert_bytes_match(src1.buffer, dest.buffer + offset, src1.len);
        }

        /* source buffers unchanged */
        assert(src0.buffer == old_src0.buffer);
        assert(src0.len == old_src0.len);
        assert(src0.capacity == old_src0.capacity);
        assert(src0.allocator == old_src0.allocator);
        if (src0.buffer != NULL && src0.len > 0) {
            assert_byte_from_buffer_matches(src0.buffer, &src0_store);
        }

        assert(src1.buffer == old_src1.buffer);
        assert(src1.len == old_src1.len);
        assert(src1.capacity == old_src1.capacity);
        assert(src1.allocator == old_src1.allocator);
        if (src1.buffer != NULL && src1.len > 0) {
            assert_byte_from_buffer_matches(src1.buffer, &src1_store);
        }
    } else {
        /* on failure dest unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        if (dest.buffer != NULL && dest.len > 0) {
            assert_byte_from_buffer_matches(dest.buffer, &dest_store);
        }

        /* source buffers unchanged */
        assert(src0.buffer == old_src0.buffer);
        assert(src0.len == old_src0.len);
        assert(src0.capacity == old_src0.capacity);
        assert(src0.allocator == old_src0.allocator);
        if (src0.buffer != NULL && src0.len > 0) {
            assert_byte_from_buffer_matches(src0.buffer, &src0_store);
        }

        assert(src1.buffer == old_src1.buffer);
        assert(src1.len == old_src1.len);
        assert(src1.capacity == old_src1.capacity);
        assert(src1.allocator == old_src1.allocator);
        if (src1.buffer != NULL && src1.len > 0) {
            assert_byte_from_buffer_matches(src1.buffer, &src1_store);
        }
    }

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src0));
    assert(aws_byte_buf_is_valid(&src1));
}
