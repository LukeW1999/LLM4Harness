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

    /* save old destination state */
    struct aws_byte_buf old_dest = dest;

    /* source buffers */
    struct aws_byte_buf src[MAX_ARGS];
    struct aws_byte_buf old_src[MAX_ARGS];

    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
        old_src[i] = src[i];
    }

    /* nondet number of arguments, bounded and >1 as required by spec */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args > 1 && number_of_args <= MAX_ARGS);

    /* compute total length of all source buffers (used for post‑condition checks) */
    size_t total_src_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        total_src_len += src[i].len;
    }
    __CPROVER_assume(total_src_len <= MAX_BUFFER_SIZE);

    /* call the function under test */
    int result = aws_byte_buf_cat(&dest,
                                 number_of_args,
                                 &src[0],
                                 &src[1],
                                 &src[2],
                                 &src[3],
                                 &src[4]);

    /* validity invariant must always hold */
    assert(aws_byte_buf_is_valid(&dest));

    /* allocator must never change */
    assert(dest.allocator == old_dest.allocator);

    /* source buffers must remain unchanged */
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(src[i].len == old_src[i].len);
        assert(src[i].capacity == old_src[i].capacity);
        assert(src[i].buffer == old_src[i].buffer);
        assert(src[i].allocator == old_src[i].allocator);
    }

    if (result == AWS_OP_SUCCESS) {
        /* on success the destination length must be increased by the sum of all source lengths */
        assert(dest.len == old_dest.len + total_src_len);
    } else {
        /* on failure the destination length cannot exceed the amount that would have been copied */
        assert(dest.len <= old_dest.len + total_src_len);
        /* and it cannot be less than the original length */
        assert(dest.len >= old_dest.len);
    }
}
