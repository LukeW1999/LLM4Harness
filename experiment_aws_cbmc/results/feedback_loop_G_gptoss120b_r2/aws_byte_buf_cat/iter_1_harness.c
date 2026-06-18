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
    /* 1. Destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Save old state of destination */
    struct aws_byte_buf old_dest = dest;

    /* 2. Source buffers */
    struct aws_byte_buf *src_bufs[MAX_ARGS];
    size_t i;
    for (i = 0; i < MAX_ARGS; ++i) {
        src_bufs[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(src_bufs[i] != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(src_bufs[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(src_bufs[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(src_bufs[i]));
    }

    /* Save old state of each source buffer */
    struct aws_byte_buf old_src[MAX_ARGS];
    for (i = 0; i < MAX_ARGS; ++i) {
        old_src[i] = *src_bufs[i];
    }

    /* 3. Nondeterministic number of arguments (bounded) */
    size_t num_args = nondet_size_t();
    __CPROVER_assume(num_args <= MAX_ARGS);

    /* 4. Call function under test with appropriate varargs */
    int result;
    if (num_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (num_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, src_bufs[0]);
    } else if (num_args == 2) {
        result = aws_byte_buf_cat(&dest, 2, src_bufs[0], src_bufs[1]);
    } else { /* num_args == 3 */
        result = aws_byte_buf_cat(&dest, 3, src_bufs[0], src_bufs[1], src_bufs[2]);
    }

    /* 5. Compute total length of all source buffers */
    size_t total_src_len = 0;
    for (i = 0; i < num_args; ++i) {
        total_src_len += src_bufs[i]->len;
    }

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Destination length increased by sum of source lengths */
        assert(dest.len == old_dest.len + total_src_len);
        /* Destination capacity, buffer pointer and allocator unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.allocator == old_dest.allocator);
        /* Source buffers unchanged */
        for (i = 0; i < num_args; ++i) {
            assert(src_bufs[i]->len == old_src[i].len);
            assert(src_bufs[i]->capacity == old_src[i].capacity);
            assert(src_bufs[i]->buffer == old_src[i].buffer);
            assert(src_bufs[i]->allocator == old_src[i].allocator);
        }
    } else {
        /* On failure destination unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.allocator == old_dest.allocator);
        /* Source buffers unchanged */
        for (i = 0; i < num_args; ++i) {
            assert(src_bufs[i]->len == old_src[i].len);
            assert(src_bufs[i]->capacity == old_src[i].capacity);
            assert(src_bufs[i]->buffer == old_src[i].buffer);
            assert(src_bufs[i]->allocator == old_src[i].allocator);
        }
    }

    /* 7. Invariant that destination remains valid */
    assert(aws_byte_buf_is_valid(&dest));

    /* 8. Invariants that each source buffer remains valid */
    for (i = 0; i < MAX_ARGS; ++i) {
        assert(aws_byte_buf_is_valid(src_bufs[i]));
    }

    /* 9. Clean up allocated source buffers */
    for (i = 0; i < MAX_ARGS; ++i) {
        free(src_bufs[i]);
    }
}
