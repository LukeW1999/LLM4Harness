#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 256
#define MAX_ARGS 5

void aws_byte_buf_cat_harness(void) {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(dest.len <= dest.capacity);

    /* save old state of destination */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_dest_content;
    save_byte_from_array(dest.buffer, dest.capacity, &old_dest_content);

    /* number of arguments (must be >= 1) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 1);
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* source buffers */
    struct aws_byte_buf src_bufs[MAX_ARGS];
    struct aws_byte_buf old_src_bufs[MAX_ARGS];
    struct store_byte_from_buffer old_src_content[MAX_ARGS];

    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src_bufs[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src_bufs[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src_bufs[i]));
        __CPROVER_assume(src_bufs[i].len <= src_bufs[i].capacity);

        old_src_bufs[i] = src_bufs[i];
        save_byte_from_array(src_bufs[i].buffer, src_bufs[i].capacity, &old_src_content[i]);
    }

    /* total length of data that could be appended */
    size_t total_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        total_len += src_bufs[i].len;
    }

    /* call the function under test with the correct number of varargs */
    int result = AWS_OP_ERR;
    switch (number_of_args) {
        case 1:
            result = aws_byte_buf_cat(&dest, number_of_args, &src_bufs[0]);
            break;
        case 2:
            result = aws_byte_buf_cat(&dest,
                                      number_of_args,
                                      &src_bufs[0],
                                      &src_bufs[1]);
            break;
        case 3:
            result = aws_byte_buf_cat(&dest,
                                      number_of_args,
                                      &src_bufs[0],
                                      &src_bufs[1],
                                      &src_bufs[2]);
            break;
        case 4:
            result = aws_byte_buf_cat(&dest,
                                      number_of_args,
                                      &src_bufs[0],
                                      &src_bufs[1],
                                      &src_bufs[2],
                                      &src_bufs[3]);
            break;
        case 5:
            result = aws_byte_buf_cat(&dest,
                                      number_of_args,
                                      &src_bufs[0],
                                      &src_bufs[1],
                                      &src_bufs[2],
                                      &src_bufs[3],
                                      &src_bufs[4]);
            break;
        default:
            __CPROVER_assert(0, "unreachable number_of_args");
    }

    /* -------------------------------------------------------------------- */
    /* Unchanged fields of destination */
    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    /* Unchanged fields and contents of each source buffer */
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(src_bufs[i].buffer == old_src_bufs[i].buffer);
        assert(src_bufs[i].capacity == old_src_bufs[i].capacity);
        assert(src_bufs[i].allocator == old_src_bufs[i].allocator);
        assert(src_bufs[i].len == old_src_bufs[i].len);
        assert_bytes_match(src_bufs[i].buffer,
                           old_src_bufs[i].buffer,
                           src_bufs[i].capacity);
    }

    /* -------------------------------------------------------------------- */
    /* Post‑conditions depending on the return value */
    if (result == AWS_OP_SUCCESS) {
        /* all data appended */
        assert(dest.len == old_dest.len + total_len);

        /* concatenated contents are correct */
        size_t offset = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            assert_bytes_match(dest.buffer + offset,
                               src_bufs[i].buffer,
                               src_bufs[i].len);
            offset += src_bufs[i].len;
        }
    } else {
        /* on failure some prefix may have been appended */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= old_dest.len + total_len);
    }

    /* -------------------------------------------------------------------- */
    /* Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(aws_byte_buf_is_valid(&src_bufs[i]));
    }
}
