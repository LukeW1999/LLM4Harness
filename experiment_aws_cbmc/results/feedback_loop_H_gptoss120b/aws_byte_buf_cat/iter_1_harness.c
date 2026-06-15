#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_ARGS 3

void aws_byte_buf_cat_harness(void) {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* save old state of dest */
    struct aws_byte_buf old_dest = dest;

    /* source buffers */
    struct aws_byte_buf src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
    }

    /* save old state of each source buffer */
    struct aws_byte_buf old_src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        old_src[i] = src[i];
    }

    /* nondeterministic number of arguments, bounded by MAX_ARGS */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* call function under test */
    int result = aws_byte_buf_cat(&dest,
                                 number_of_args,
                                 &src[0],
                                 &src[1],
                                 &src[2]);

    /* compute total length of data that should have been appended on success */
    size_t total_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        total_len += src[i].len;
    }

    if (result == AWS_OP_SUCCESS) {
        /* dest length increased by total_len */
        assert(dest.len == old_dest.len + total_len);
        /* dest capacity, allocator and buffer pointer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        /* source buffers unchanged */
        for (size_t i = 0; i < MAX_ARGS; ++i) {
            assert(src[i].len == old_src[i].len);
            assert(src[i].capacity == old_src[i].capacity);
            assert(src[i].allocator == old_src[i].allocator);
            assert(src[i].buffer == old_src[i].buffer);
        }
    } else {
        /* on failure dest unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        /* source buffers unchanged */
        for (size_t i = 0; i < MAX_ARGS; ++i) {
            assert(src[i].len == old_src[i].len);
            assert(src[i].capacity == old_src[i].capacity);
            assert(src[i].allocator == old_src[i].allocator);
            assert(src[i].buffer == old_src[i].buffer);
        }
    }

    /* validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(aws_byte_buf_is_valid(&src[i]));
    }
}
