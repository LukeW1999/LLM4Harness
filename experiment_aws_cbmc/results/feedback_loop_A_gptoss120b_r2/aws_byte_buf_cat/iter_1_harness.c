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

#define MAX_ARGS 2

void aws_byte_buf_cat_harness(void) {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* save old destination */
    struct aws_byte_buf old_dest = dest;

    /* source buffers */
    struct aws_byte_buf *src[MAX_ARGS];
    struct aws_byte_buf old_src[MAX_ARGS];

    for (size_t i = 0; i < MAX_ARGS; ++i) {
        src[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(src[i] != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(src[i]));
        old_src[i] = *src[i];
    }

    /* nondet number of arguments */
    size_t num_args = nondet_size_t();
    __CPROVER_assume(num_args <= MAX_ARGS);

    /* compute total length of all source buffers */
    size_t total_len = 0;
    for (size_t i = 0; i < num_args; ++i) {
        total_len += src[i]->len;
    }
    /* avoid overflow when adding to old_dest.len */
    __CPROVER_assume(old_dest.len <= SIZE_MAX - total_len);

    /* call the function under test */
    int result;
    if (num_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (num_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, src[0]);
    } else {
        result = aws_byte_buf_cat(&dest, 2, src[0], src[1]);
    }

    /* postconditions */

    /* unchanged fields of destination */
    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* all appends succeeded, length increased by total_len */
        assert(dest.len == old_dest.len + total_len);
    } else {
        /* on failure, length is non‑decreasing and never exceeds old + total_len */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= old_dest.len + total_len);
    }

    /* source buffers must remain unchanged */
    for (size_t i = 0; i < num_args; ++i) {
        assert(src[i]->len == old_src[i].len);
        assert(src[i]->capacity == old_src[i].capacity);
        assert(src[i]->buffer == old_src[i].buffer);
        assert(src[i]->allocator == old_src[i].allocator);
        assert(aws_byte_buf_is_valid(src[i]));
    }

    /* validity of destination after the call */
    assert(aws_byte_buf_is_valid(&dest));
}
