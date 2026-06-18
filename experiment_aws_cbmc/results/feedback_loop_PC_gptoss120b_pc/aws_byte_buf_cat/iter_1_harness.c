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

    /* snapshot of destination before the call */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_dest_bytes;
    save_byte_from_array(dest.buffer, dest.len, &old_dest_bytes);

    /* nondet number of arguments, bounded to keep the harness tractable */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= 5);

    /* source buffers */
    struct aws_byte_buf src[5];
    struct aws_byte_buf old_src[5];
    struct store_byte_from_buffer old_src_bytes[5];

    for (size_t i = 0; i < 5; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));

        old_src[i] = src[i];
        save_byte_from_array(src[i].buffer, src[i].len, &old_src_bytes[i]);
    }

    /* call the function under test with the appropriate var‑args */
    int result;
    if (number_of_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (number_of_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, &src[0]);
    } else if (number_of_args == 2) {
        result = aws_byte_buf_cat(&dest, 2, &src[0], &src[1]);
    } else if (number_of_args == 3) {
        result = aws_byte_buf_cat(&dest, 3, &src[0], &src[1], &src[2]);
    } else if (number_of_args == 4) {
        result = aws_byte_buf_cat(&dest, 4, &src[0], &src[1], &src[2], &src[3]);
    } else { /* number_of_args == 5 */
        result = aws_byte_buf_cat(&dest, 5, &src[0], &src[1], &src[2], &src[3], &src[4]);
    }

    /* post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest length increased by the sum of the source lengths */
        size_t expected_len = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            expected_len += src[i].len;
        }
        assert(dest.len == expected_len);

        /* dest capacity, allocator and buffer pointer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);

        /* original bytes of dest unchanged */
        assert_byte_from_buffer_matches(old_dest.buffer, &old_dest_bytes);

        /* concatenated bytes appear in order after the original data */
        size_t offset = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            assert_bytes_match(dest.buffer + offset, src[i].buffer, src[i].len);
            offset += src[i].len;
        }
    } else {
        /* on error dest must be unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert_byte_from_buffer_matches(dest.buffer, &old_dest_bytes);
    }

    /* source buffers must be unchanged regardless of result */
    for (size_t i = 0; i < 5; ++i) {
        assert(src[i].len == old_src[i].len);
        assert(src[i].capacity == old_src[i].capacity);
        assert(src[i].allocator == old_src[i].allocator);
        assert(src[i].buffer == old_src[i].buffer);
        assert_byte_from_buffer_matches(src[i].buffer, &old_src_bytes[i]);
    }

    /* validity invariant */
    assert(aws_byte_buf_is_valid(&dest));
}
