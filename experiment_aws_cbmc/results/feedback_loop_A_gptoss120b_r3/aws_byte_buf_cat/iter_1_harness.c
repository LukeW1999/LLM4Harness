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

#define MAX_ARGS 3

void aws_byte_buf_cat_harness(void) {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    struct aws_byte_buf old_dest = dest;

    /* number of source buffers (must be > 1) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2);
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* source buffers */
    struct aws_byte_buf src_bufs[MAX_ARGS];
    struct aws_byte_buf old_src[MAX_ARGS];

    for (size_t i = 0; i < number_of_args; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src_bufs[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src_bufs[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src_bufs[i]));
        old_src[i] = src_bufs[i];
    }

    /* compute total length of all source buffers, avoiding overflow */
    size_t total_src_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        __CPROVER_assume(total_src_len <= SIZE_MAX - src_bufs[i].len);
        total_src_len += src_bufs[i].len;
    }

    int result;
    if (number_of_args == 2) {
        result = aws_byte_buf_cat(&dest, 2,
                                  &src_bufs[0],
                                  &src_bufs[1]);
    } else if (number_of_args == 3) {
        result = aws_byte_buf_cat(&dest, 3,
                                  &src_bufs[0],
                                  &src_bufs[1],
                                  &src_bufs[2]);
    } else {
        /* unreachable due to assumptions */
        result = AWS_OP_ERR;
    }

    /* post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* on success all source data is appended */
        assert(dest.len == old_dest.len + total_src_len);
    } else {
        /* on failure some prefix may have been appended */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= old_dest.len + total_src_len);
    }

    /* fields of dest that must not change */
    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    /* source buffers must remain unchanged */
    for (size_t i = 0; i < number_of_args; ++i) {
        assert(src_bufs[i].len == old_src[i].len);
        assert(src_bufs[i].capacity == old_src[i].capacity);
        assert(src_bufs[i].buffer == old_src[i].buffer);
        assert(src_bufs[i].allocator == old_src[i].allocator);
    }

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < number_of_args; ++i) {
        assert(aws_byte_buf_is_valid(&src_bufs[i]));
    }
}
