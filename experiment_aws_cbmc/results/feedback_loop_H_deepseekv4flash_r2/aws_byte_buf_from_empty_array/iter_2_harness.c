#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

extern struct aws_allocator *aws_default_allocator(void);

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic source buffer */
    struct aws_byte_buf *src = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(src != NULL);

    src->capacity = nondet_size_t();
    __CPROVER_assume(src->capacity <= MAX_BUFFER_SIZE);
    src->len = nondet_size_t();
    __CPROVER_assume(src->len <= src->capacity);

    ensure_byte_buf_has_allocated_buffer_member(src);
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save source state for immutability check */
    struct store_byte_from_buffer saved_src_byte;
    if (src->buffer != NULL && src->len > 0) {
        save_byte_from_array(src->buffer, src->len, &saved_src_byte);
    }

    /* Destination and allocator */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Call function */
    int result = aws_byte_buf_init_copy(&dest, allocator, src);

    /* Postconditions */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(src));

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src->len);
        assert(dest.capacity == src->capacity);
        assert(dest.allocator == allocator);

        if (src->buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.buffer != src->buffer);
            if (src->len > 0) {
                assert_bytes_match(dest.buffer, src->buffer, src->len);
            }
        }
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    if (src->buffer != NULL && src->len > 0) {
        assert_byte_from_buffer_matches(src->buffer, &saved_src_byte);
    }

    free(src->buffer);
    free(src);
}
