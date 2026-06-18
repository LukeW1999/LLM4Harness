#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_cat_harness() {
    /* number of source buffers, between 2 and 4 */
    size_t number_of_args;
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= 4);

    /* ---------- destination buffer ---------- */
    struct aws_byte_buf dest;
    dest.allocator = aws_default_allocator();
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* save old state */
    size_t old_len = dest.len;
    size_t old_capacity = dest.capacity;
    uint8_t *old_buffer = dest.buffer;
    struct aws_allocator *old_allocator = dest.allocator;

    /* ---------- source buffers ---------- */
    struct aws_byte_buf *buffers[4];
    struct aws_byte_buf old_buffers[4];
    uint8_t *old_buffer_ptrs[4];
    struct aws_allocator *old_buffer_allocators[4];

    for (size_t i = 0; i < number_of_args; ++i) {
        buffers[i] = malloc(sizeof(struct aws_byte_buf));
        buffers[i]->allocator = aws_default_allocator();
        ensure_byte_buf_has_allocated_buffer_member(buffers[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(buffers[i]));
        __CPROVER_assume(aws_byte_buf_is_bounded(buffers[i], MAX_BUFFER_SIZE));

        old_buffers[i] = *buffers[i];
        old_buffer_ptrs[i] = buffers[i]->buffer;
        old_buffer_allocators[i] = buffers[i]->allocator;
    }

    /* ---------- call function under test ---------- */
    int result;
    if (number_of_args == 2) {
        result = aws_byte_buf_cat(&dest, number_of_args, buffers[0], buffers[1]);
    } else if (number_of_args == 3) {
        result = aws_byte_buf_cat(&dest, number_of_args, buffers[0], buffers[1], buffers[2]);
    } else {
        result = aws_byte_buf_cat(&dest, number_of_args, buffers[0], buffers[1], buffers[2], buffers[3]);
    }

    /* ---------- postconditions ---------- */
    if (result == AWS_OP_SUCCESS) {
        size_t total_src_len = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            total_src_len += old_buffers[i].len;
        }
        /* dest length must increase by the sum of source lengths */
        assert(dest.len == old_len + total_src_len);
        /* allocator never changes */
        assert(dest.allocator == old_allocator);

        /* dest must remain or become valid */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* on failure dest is completely unchanged */
        assert(dest.len == old_len);
        assert(dest.buffer == old_buffer);
        assert(dest.capacity == old_capacity);
        assert(dest.allocator == old_allocator);
    }

    /* source buffers must never be modified */
    for (size_t i = 0; i < number_of_args; ++i) {
        assert(buffers[i]->len == old_buffers[i].len);
        assert(buffers[i]->buffer == old_buffer_ptrs[i]);
        assert(buffers[i]->capacity == old_buffers[i].capacity);
        assert(buffers[i]->allocator == old_buffer_allocators[i]);
        assert(aws_byte_buf_is_valid(buffers[i]));
    }

    /* ---------- cleanup ---------- */
    for (size_t i = 0; i < number_of_args; ++i) {
        if (buffers[i]) {
            free(buffers[i]->buffer);
            free(buffers[i]);
        }
    }
}
