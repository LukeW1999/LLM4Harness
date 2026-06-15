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

#define MAX_ARGS 5

void aws_byte_buf_cat_harness(void) {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* save old state of dest */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_dest_bytes;
    save_byte_from_array(dest.buffer, dest.capacity, &old_dest_bytes);

    /* source buffers */
    struct aws_byte_buf src[MAX_ARGS];
    struct aws_byte_buf old_src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
        old_src[i] = src[i];
    }

    /* nondet number of arguments, must be >1 per Doxygen */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2);
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* call function under test */
    int result = aws_byte_buf_cat(
        &dest,
        number_of_args,
        &src[0],
        &src[1],
        &src[2],
        &src[3],
        &src[4]);

    /* post‑conditions */

    /* validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(aws_byte_buf_is_valid(&src[i]));
    }

    /* unchanged fields on both paths */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(src[i].allocator == old_src[i].allocator);
        assert(src[i].capacity == old_src[i].capacity);
        assert(src[i].buffer == old_src[i].buffer);
    }

    if (result == AWS_OP_SUCCESS) {
        /* compute total length of all appended source buffers */
        size_t total_len = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            total_len += src[i].len;
        }

        /* dest length increased by total_len */
        assert(dest.len == old_dest.len + total_len);

        /* verify that the appended bytes match the source data */
        size_t offset = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            if (src[i].len > 0) {
                assert_bytes_match(dest.buffer + offset, src[i].buffer, src[i].len);
            }
            offset += src[i].len;
        }

        /* source buffers unchanged */
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(src[i].len == old_src[i].len);
            assert(src[i].capacity == old_src[i].capacity);
            assert(src[i].buffer == old_src[i].buffer);
        }
    } else {
        /* on failure dest must be unchanged */
        assert(dest.len == old_dest.len);
        assert_byte_from_buffer_matches(dest.buffer, &old_dest_bytes);

        /* source buffers unchanged */
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(src[i].len == old_src[i].len);
            assert(src[i].capacity == old_src[i].capacity);
            assert(src[i].buffer == old_src[i].buffer);
        }
    }
}
