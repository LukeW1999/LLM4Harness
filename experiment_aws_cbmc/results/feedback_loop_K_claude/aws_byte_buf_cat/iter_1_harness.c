#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the number of variadic args for tractability */
#define MAX_ARGS 2

int aws_byte_buf_cat(struct aws_byte_buf *dest, size_t number_of_args, ...);

void aws_byte_buf_cat_harness(void) {
    /* ------------------------------------------------------------------ */
    /* Symbolic inputs                                                      */
    /* ------------------------------------------------------------------ */

    /* Destination buffer */
    struct aws_byte_buf *dest = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(dest != NULL);

    size_t dest_capacity;
    size_t dest_len;
    __CPROVER_assume(dest_capacity <= 64);   /* bound for tractability */
    __CPROVER_assume(dest_len <= dest_capacity);

    uint8_t *dest_buffer = malloc(dest_capacity);
    __CPROVER_assume(dest_capacity == 0 || dest_buffer != NULL);

    dest->buffer    = dest_buffer;
    dest->len       = dest_len;
    dest->capacity  = dest_capacity;
    dest->allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(dest));

    /* Number of source buffers: 1 or 2 */
    size_t number_of_args;
    __CPROVER_assume(number_of_args >= 1 && number_of_args <= MAX_ARGS);

    /* Source buffer 1 */
    struct aws_byte_buf *src1 = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(src1 != NULL);

    size_t src1_capacity;
    size_t src1_len;
    __CPROVER_assume(src1_capacity <= 64);
    __CPROVER_assume(src1_len <= src1_capacity);

    uint8_t *src1_buffer = malloc(src1_capacity);
    __CPROVER_assume(src1_capacity == 0 || src1_buffer != NULL);

    src1->buffer    = src1_buffer;
    src1->len       = src1_len;
    src1->capacity  = src1_capacity;
    src1->allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(src1));

    /* Source buffer 2 (only used when number_of_args == 2) */
    struct aws_byte_buf *src2 = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(src2 != NULL);

    size_t src2_capacity;
    size_t src2_len;
    __CPROVER_assume(src2_capacity <= 64);
    __CPROVER_assume(src2_len <= src2_capacity);

    uint8_t *src2_buffer = malloc(src2_capacity);
    __CPROVER_assume(src2_capacity == 0 || src2_buffer != NULL);

    src2->buffer    = src2_buffer;
    src2->len       = src2_len;
    src2->capacity  = src2_capacity;
    src2->allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(src2));

    /* Ensure no aliasing between dest backing memory and source backing memory */
    __CPROVER_assume(dest_buffer != src1_buffer);
    __CPROVER_assume(dest_buffer != src2_buffer);
    __CPROVER_assume(src1_buffer != src2_buffer);

    /* ------------------------------------------------------------------ */
    /* Record pre-call state                                                */
    /* ------------------------------------------------------------------ */
    size_t old_dest_len      = dest->len;
    size_t old_dest_capacity = dest->capacity;
    struct aws_allocator *old_dest_allocator = dest->allocator;
    uint8_t *old_dest_buffer_ptr = dest->buffer;

    size_t old_src1_len      = src1->len;
    size_t old_src1_capacity = src1->capacity;
    uint8_t *old_src1_buffer_ptr = src1->buffer;

    size_t old_src2_len      = src2->len;
    size_t old_src2_capacity = src2->capacity;
    uint8_t *old_src2_buffer_ptr = src2->buffer;

    /* ------------------------------------------------------------------ */
    /* Call the function under test                                         */
    /* ------------------------------------------------------------------ */
    int result;
    if (number_of_args == 1) {
        result = aws_byte_buf_cat(dest, 1, src1);
    } else {
        result = aws_byte_buf_cat(dest, 2, src1, src2);
    }

    /* ------------------------------------------------------------------ */
    /* Postcondition assertions                                             */
    /* ------------------------------------------------------------------ */

    /* Return value must be 0 (success) or -1 (error) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* dest must still be a valid aws_byte_buf */
    assert(aws_byte_buf_is_valid(dest));

    /* dest->capacity must not change */
    assert(dest->capacity == old_dest_capacity);

    /* dest->allocator must not change */
    assert(dest->allocator == old_dest_allocator);

    /* dest->buffer pointer must not change */
    assert(dest->buffer == old_dest_buffer_ptr);

    /* dest->len must not exceed capacity */
    assert(dest->len <= dest->capacity);

    /* dest->len must be >= old_dest_len (we only append, never shrink) */
    assert(dest->len >= old_dest_len);

    /* On success with 1 arg: dest->len == old_dest_len + src1->len */
    if (result == AWS_OP_SUCCESS && number_of_args == 1) {
        assert(dest->len == old_dest_len + old_src1_len);
    }

    /* On success with 2 args: dest->len == old_dest_len + src1->len + src2->len */
    if (result == AWS_OP_SUCCESS && number_of_args == 2) {
        assert(dest->len == old_dest_len + old_src1_len + old_src2_len);
    }

    /* On success: the total appended bytes must have fit in capacity */
    if (result == AWS_OP_SUCCESS) {
        assert(dest->len <= dest->capacity);
    }

    /* Source buffers must not be modified by the call */
    assert(src1->len      == old_src1_len);
    assert(src1->capacity == old_src1_capacity);
    assert(src1->buffer   == old_src1_buffer_ptr);

    assert(src2->len      == old_src2_len);
    assert(src2->capacity == old_src2_capacity);
    assert(src2->buffer   == old_src2_buffer_ptr);

    /* If there was not enough space for src1 alone, result must be error */
    if (old_dest_capacity - old_dest_len < old_src1_len) {
        assert(result == AWS_OP_ERR);
    }

    /* If there was enough space for all args, result must be success */
    if (number_of_args == 1) {
        if (old_dest_capacity - old_dest_len >= old_src1_len) {
            assert(result == AWS_OP_SUCCESS);
        }
    } else {
        /* number_of_args == 2 */
        /* Check for overflow before comparing */
        if (old_src1_len <= old_dest_capacity - old_dest_len) {
            size_t remaining_after_src1 = old_dest_capacity - old_dest_len - old_src1_len;
            if (old_src2_len <= remaining_after_src1) {
                assert(result == AWS_OP_SUCCESS);
            }
        }
    }
}
