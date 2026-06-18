#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* number_of_args: at least 1, bounded to keep state space manageable */
    size_t number_of_args;
    __CPROVER_assume(number_of_args >= 1);
    __CPROVER_assume(number_of_args <= 4); /* small bound for performance */

    /* We need to model the variadic arguments.
     * We'll create an array of byte_buf pointers and pass them as varargs.
     * CBMC can't directly handle va_list in a harness, so we model the
     * function's behavior by calling aws_byte_buf_append in a loop.
     * However, the task is to write a harness for aws_byte_buf_cat itself.
     * We'll use a trick: create the buffers, then call the function with
     * the actual variadic arguments.
     */

    /* Create the source buffers */
    struct aws_byte_buf *src_bufs[4];
    for (size_t i = 0; i < number_of_args; i++) {
        src_bufs[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(src_bufs[i] != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(src_bufs[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(src_bufs[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(src_bufs[i]));
    }

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_dest_byte;
    if (dest.buffer != NULL && dest.capacity > 0) {
        save_byte_from_array(dest.buffer, dest.capacity, &old_dest_byte);
    }

    /* Save old states of source buffers */
    struct aws_byte_buf old_src_bufs[4];
    struct store_byte_from_buffer old_src_bytes[4];
    for (size_t i = 0; i < number_of_args; i++) {
        old_src_bufs[i] = *src_bufs[i];
        if (src_bufs[i]->buffer != NULL && src_bufs[i]->capacity > 0) {
            save_byte_from_array(src_bufs[i]->buffer, src_bufs[i]->capacity, &old_src_bytes[i]);
        }
    }

    /* 3. Call function under test with variadic arguments */
    int result;
    switch (number_of_args) {
        case 1:
            result = aws_byte_buf_cat(&dest, 1, src_bufs[0]);
            break;
        case 2:
            result = aws_byte_buf_cat(&dest, 2, src_bufs[0], src_bufs[1]);
            break;
        case 3:
            result = aws_byte_buf_cat(&dest, 3, src_bufs[0], src_bufs[1], src_bufs[2]);
            break;
        case 4:
            result = aws_byte_buf_cat(&dest, 4, src_bufs[0], src_bufs[1], src_bufs[2], src_bufs[3]);
            break;
        default:
            result = aws_byte_buf_cat(&dest, number_of_args, src_bufs[0]);
            break;
    }

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest->len should equal old_dest.len + sum of all src lens */
        size_t total_src_len = 0;
        for (size_t i = 0; i < number_of_args; i++) {
            total_src_len += old_src_bufs[i].len;
        }
        assert(dest.len == old_dest.len + total_src_len);

        /* The appended data should match the source buffers' contents */
        size_t offset = old_dest.len;
        for (size_t i = 0; i < number_of_args; i++) {
            if (old_src_bufs[i].len > 0) {
                assert_bytes_match(dest.buffer + offset, old_src_bufs[i].buffer, old_src_bufs[i].len);
                offset += old_src_bufs[i].len;
            }
        }
    } else {
        /* On failure (AWS_OP_ERR), dest should be unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        if (old_dest.buffer != NULL && old_dest.capacity > 0) {
            assert_byte_from_buffer_matches(dest.buffer, &old_dest_byte);
        }
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    /* buffer pointer may change if reallocated, but on failure it must not */
    if (result != AWS_OP_SUCCESS) {
        assert(dest.buffer == old_dest.buffer);
    }

    /* Source buffers must remain unchanged */
    for (size_t i = 0; i < number_of_args; i++) {
        assert(src_bufs[i]->len == old_src_bufs[i].len);
        assert(src_bufs[i]->capacity == old_src_bufs[i].capacity);
        assert(src_bufs[i]->allocator == old_src_bufs[i].allocator);
        if (old_src_bufs[i].buffer != NULL && old_src_bufs[i].capacity > 0) {
            assert_byte_from_buffer_matches(src_bufs[i]->buffer, &old_src_bytes[i]);
        }
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < number_of_args; i++) {
        assert(aws_byte_buf_is_valid(src_bufs[i]));
    }

    /* Cleanup */
    for (size_t i = 0; i < number_of_args; i++) {
        free(src_bufs[i]);
    }
}
