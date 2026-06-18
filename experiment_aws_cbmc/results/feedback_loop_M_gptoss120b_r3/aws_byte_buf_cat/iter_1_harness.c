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

    /* 2. Source buffers (array of MAX_ARGS) */
    struct aws_byte_buf src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
    }

    /* 3. Number of arguments (bounded) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* 4. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        old_src[i] = src[i];
    }

    /* 5. Compute total length of all source buffers that will be concatenated */
    size_t total_src_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        total_src_len += src[i].len;
    }

    /* 6. Call function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args,
                                  &src[0],
                                  &src[1],
                                  &src[2]);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest.len increased by the sum of source lengths */
        assert(dest.len == old_dest.len + total_src_len);
        /* dest.capacity, allocator and buffer pointer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
    } else {
        /* On failure dest must be unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
    }

    /* Source buffers must remain unchanged regardless of result */
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(src[i].len == old_src[i].len);
        assert(src[i].capacity == old_src[i].capacity);
        assert(src[i].allocator == old_src[i].allocator);
        assert(src[i].buffer == old_src[i].buffer);
    }

    /* 8. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(aws_byte_buf_is_valid(&src[i]));
    }
}
