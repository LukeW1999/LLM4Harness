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

    /* source buffers */
    struct aws_byte_buf srcs[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&srcs[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&srcs[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&srcs[i]));
    }

    /* number of arguments (must be > 1 as per Doxygen) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args > 1);
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_srcs[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        old_srcs[i] = srcs[i];
    }

    /* call function (extra arguments are ignored) */
    int result = aws_byte_buf_cat(&dest,
                                  number_of_args,
                                  &srcs[0],
                                  &srcs[1],
                                  &srcs[2],
                                  &srcs[3],
                                  &srcs[4]);

    /* compute total length of all source buffers that should be concatenated */
    size_t total_src_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        total_src_len += srcs[i].len;
    }

    /* post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* all data copied */
        assert(dest.len == old_dest.len + total_src_len);
    } else {
        /* some data may have been copied, but never more than total */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= old_dest.len + total_src_len);
    }

    /* fields that never change */
    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    /* source buffers must remain unchanged */
    for (size_t i = 0; i < number_of_args; ++i) {
        assert(srcs[i].len == old_srcs[i].len);
        assert(srcs[i].capacity == old_srcs[i].capacity);
        assert(srcs[i].buffer == old_srcs[i].buffer);
        assert(srcs[i].allocator == old_srcs[i].allocator);
    }

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < number_of_args; ++i) {
        assert(aws_byte_buf_is_valid(&srcs[i]));
    }
}
