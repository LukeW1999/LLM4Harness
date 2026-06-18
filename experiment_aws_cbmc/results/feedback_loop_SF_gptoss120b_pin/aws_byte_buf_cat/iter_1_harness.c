#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ARGS 5

void aws_byte_buf_cat_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* destination buffer */
    struct aws_byte_buf dest;
    dest.allocator = alloc;
    ensure_byte_buf_is_valid(&dest);

    /* number of source buffers (varargs) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* source buffers */
    struct aws_byte_buf src_bufs[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        src_bufs[i].allocator = alloc;
        ensure_byte_buf_is_valid(&src_bufs[i]);
    }

    /* pre‑call snapshot of destination */
    size_t dest_len_old = dest.len;
    size_t dest_capacity_old = dest.capacity;
    uint8_t *dest_buffer_old = dest.buffer;
    uint8_t *dest_contents_old = NULL;
    if (dest_len_old > 0) {
        dest_contents_old = malloc(dest_len_old);
        __CPROVER_assume(dest_contents_old);
        memcpy(dest_contents_old, dest.buffer, dest_len_old);
    }

    /* call the function under verification */
    int rc = aws_byte_buf_cat(
        &dest,
        number_of_args,
        &src_bufs[0],
        &src_bufs[1],
        &src_bufs[2],
        &src_bufs[3],
        &src_bufs[4]);

    /* ASSERT_POSTCONDITIONS_HERE */
    if (rc == 0) {
        /* success: length and capacity invariants */
        size_t total_src_len = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            total_src_len += src_bufs[i].len;
        }
        assert(dest.len == dest_len_old + total_src_len);
        assert(dest.capacity >= dest.len);

        /* frame condition: original destination contents unchanged */
        for (size_t i = 0; i < dest_len_old; ++i) {
            assert(dest.buffer[i] == dest_contents_old[i]);
        }

        /* frame condition: appended data matches source buffers */
        size_t offset = dest_len_old;
        for (size_t i = 0; i < number_of_args; ++i) {
            for (size_t j = 0; j < src_bufs[i].len; ++j) {
                assert(dest.buffer[offset + j] == src_bufs[i].buffer[j]);
            }
            offset += src_bufs[i].len;
        }
        assert(offset == dest.len);
    } else {
        /* failure: destination unchanged */
        assert(dest.len == dest_len_old);
        assert(dest.capacity == dest_capacity_old);
        assert(dest.buffer == dest_buffer_old);
        for (size_t i = 0; i < dest_len_old; ++i) {
            assert(dest.buffer[i] == dest_contents_old[i]);
        }
    }

    free(dest_contents_old);
}
