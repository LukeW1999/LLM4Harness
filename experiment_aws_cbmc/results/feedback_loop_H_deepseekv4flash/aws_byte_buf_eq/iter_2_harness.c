#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_eq_harness() {
    /* Non-deterministic parameters */
    struct aws_byte_buf *a = malloc(sizeof(*a));
    struct aws_byte_buf *b = malloc(sizeof(*b));

    /* Preconditions: pointers non-null */
    __CPROVER_assume(a != NULL);
    __CPROVER_assume(b != NULL);

    /* Bound the size of buffers to limit state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(a);
    ensure_byte_buf_has_allocated_buffer_member(b);
    __CPROVER_assume(aws_byte_buf_is_valid(a));
    __CPROVER_assume(aws_byte_buf_is_valid(b));

    /* Save old states for immutability check */
    struct aws_byte_buf old_a = *a;
    struct aws_byte_buf old_b = *b;

    /* Call the function */
    bool result = aws_byte_buf_eq(a, b);

    /* Postconditions */
    if (result) {
        /* If equal, lengths must match */
        assert(a->len == b->len);
        /* If both buffers have non-null buffers, contents must match */
        if (a->buffer != NULL && b->buffer != NULL) {
            assert_bytes_match(a->buffer, b->buffer, a->len);
        } else if (a->buffer == NULL && b->buffer == NULL) {
            /* Both NULL: they are equal only if both lengths are 0 */
            assert(a->len == 0);
            assert(b->len == 0);
        } else {
            /* One NULL, one non-NULL: cannot be equal */
            assert(0); // unreachable if result is true
        }
    } else {
        /* If not equal, either lengths differ or contents differ */
        if (a->len == b->len) {
            /* If lengths equal, contents must differ at some position */
            if (a->buffer != NULL && b->buffer != NULL) {
                /* There exists an index where bytes differ */
                /* We cannot directly assert existence, but we can assert that not all bytes match */
                /* For CBMC, we can use a loop to check all positions */
                bool all_match = true;
                for (size_t i = 0; i < a->len; i++) {
                    if (a->buffer[i] != b->buffer[i]) {
                        all_match = false;
                        break;
                    }
                }
                assert(!all_match);
            } else {
                /* One buffer NULL, other non-NULL: lengths equal but one buffer NULL implies len=0? Actually if len>0 and buffer NULL, that's invalid. But we assume valid buffers. So if lengths equal and one buffer NULL, the other must also be NULL and len=0. So they would be equal. So this case cannot happen when result is false. */
                assert(0); // unreachable
            }
        } else {
            /* Lengths differ, that's fine */
        }
    }

    /* Both buffers must remain unchanged */
    assert(a->len == old_a.len);
    assert(a->capacity == old_a.capacity);
    assert(a->allocator == old_a.allocator);
    if (a->buffer != NULL) {
        assert_bytes_match(a->buffer, old_a.buffer, a->len);
    } else {
        assert(a->buffer == old_a.buffer);
    }

    assert(b->len == old_b.len);
    assert(b->capacity == old_b.capacity);
    assert(b->allocator == old_b.allocator);
    if (b->buffer != NULL) {
        assert_bytes_match(b->buffer, old_b.buffer, b->len);
    } else {
        assert(b->buffer == old_b.buffer);
    }

    /* Free allocated memory */
    free(a);
    free(b);
}
