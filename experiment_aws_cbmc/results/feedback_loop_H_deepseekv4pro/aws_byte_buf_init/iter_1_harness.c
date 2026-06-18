#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    struct aws_allocator *allocator = aws_default_allocator();

    /* Bounding: capacity is a size_t, bound it to limit state space */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 
     * We don't call aws_byte_buf_is_valid(buf) before init because buf is
     * uninitialized. The function's PRECONDITION only requires buf != NULL
     * and allocator != NULL.
     */
    __CPROVER_assume(buf != NULL);
    __CPROVER_assume(allocator != NULL);

    /* 2. Save old state — not meaningful here since buf is uninitialized,
     *    but we can save the pre-call state for postcondition checks. */
    struct aws_byte_buf old = *buf;

    /* 3. Call function under test */
    int result = aws_byte_buf_init(buf, allocator, capacity);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success postconditions from Doxygen / implementation:
         * - len == 0
         * - capacity matches requested capacity
         * - allocator matches input allocator
         * - buffer is NULL iff capacity == 0
         */
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }
    } else {
        /* Failure path: result == AWS_OP_ERR (only returned when capacity>0 and alloc fails)
         * Implementation calls AWS_ZERO_STRUCT(*buf), so all fields become 0.
         */
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->buffer == NULL);
        assert(buf->allocator == NULL);
    }

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(buf));

    /* Free allocated memory to satisfy CBMC memory checks */
    if (buf->buffer != NULL && buf->capacity > 0) {
        free(buf->buffer);
    }
    free(buf);
}
