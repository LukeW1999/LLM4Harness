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
    /* Non-deterministically choose number of args between 2 and some small bound */
    size_t number_of_args;
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= 4);

    /* Destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Save old state of dest */
    struct aws_byte_buf old_dest = dest;
    size_t old_len = dest.len;
    size_t old_capacity = dest.capacity;
    uint8_t *old_buffer = dest.buffer;
    struct aws_allocator *old_allocator = dest.allocator;

    /* Prepare source byte_bufs */
    struct aws_byte_buf *buffers[4];
    struct aws_byte_buf old_buffers[4];
    uint8_t *old_buffer_ptrs[4];
    struct aws_allocator *old_buffer_allocators[4];

    for (size_t i = 0; i < number_of_args; ++i) {
        buffers[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(aws_byte_buf_is_bounded(buffers[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buffers[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(buffers[i]));
        old_buffers[i] = *buffers[i];
        old_buffer_ptrs[i] = buffers[i]->buffer;
        old_buffer_allocators[i] = buffers[i]->allocator;
    }

    /* Call the function under test */
    if (number_of_args == 2) {
        int result = aws_byte_buf_cat(&dest, number_of_args, buffers[0], buffers[1]);

        if (result == AWS_OP_SUCCESS) {
            /* On success, dest->len should be increased by the sum of source lengths */
            size_t total_src_len = 0;
            for (size_t i = 0; i < number_of_args; ++i) {
                total_src_len += old_buffers[i].len;
            }
            assert(dest.len == old_len + total_src_len);
        } else {
            /* On failure, dest should be unchanged */
            assert(dest.len == old_len);
            assert(dest.buffer == old_buffer);
        }

        /* Fields that never change */
        assert(dest.capacity == old_capacity);
        assert(dest.allocator == old_allocator);
        /* Source buffers must not be modified */
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(buffers[i]->len == old_buffers[i].len);
            assert(buffers[i]->buffer == old_buffer_ptrs[i]);
            assert(buffers[i]->capacity == old_buffers[i].capacity);
            assert(buffers[i]->allocator == old_buffer_allocators[i]);
        }

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&dest));
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(aws_byte_buf_is_valid(buffers[i]));
        }
    } else if (number_of_args == 3) {
        int result = aws_byte_buf_cat(&dest, number_of_args, buffers[0], buffers[1], buffers[2]);

        if (result == AWS_OP_SUCCESS) {
            size_t total_src_len = 0;
            for (size_t i = 0; i < number_of_args; ++i) {
                total_src_len += old_buffers[i].len;
            }
            assert(dest.len == old_len + total_src_len);
        } else {
            assert(dest.len == old_len);
            assert(dest.buffer == old_buffer);
        }

        assert(dest.capacity == old_capacity);
        assert(dest.allocator == old_allocator);
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(buffers[i]->len == old_buffers[i].len);
            assert(buffers[i]->buffer == old_buffer_ptrs[i]);
            assert(buffers[i]->capacity == old_buffers[i].capacity);
            assert(buffers[i]->allocator == old_buffer_allocators[i]);
        }
        assert(aws_byte_buf_is_valid(&dest));
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(aws_byte_buf_is_valid(buffers[i]));
        }
    } else {
        int result = aws_byte_buf_cat(&dest, number_of_args, buffers[0], buffers[1], buffers[2], buffers[3]);

        if (result == AWS_OP_SUCCESS) {
            size_t total_src_len = 0;
            for (size_t i = 0; i < number_of_args; ++i) {
                total_src_len += old_buffers[i].len;
            }
            assert(dest.len == old_len + total_src_len);
        } else {
            assert(dest.len == old_len);
            assert(dest.buffer == old_buffer);
        }

        assert(dest.capacity == old_capacity);
        assert(dest.allocator == old_allocator);
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(buffers[i]->len == old_buffers[i].len);
            assert(buffers[i]->buffer == old_buffer_ptrs[i]);
            assert(buffers[i]->capacity == old_buffers[i].capacity);
            assert(buffers[i]->allocator == old_buffer_allocators[i]);
        }
        assert(aws_byte_buf_is_valid(&dest));
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(aws_byte_buf_is_valid(buffers[i]));
        }
    }

    /* Free allocated source buffers */
    for (size_t i = 0; i < number_of_args; ++i) {
        free(buffers[i]);
    }
}
