#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness(void) {
    /* Allocate two aws_byte_buf structures */
    struct aws_byte_buf *a = malloc(sizeof(struct aws_byte_buf));
    struct aws_byte_buf *b = malloc(sizeof(struct aws_byte_buf));

    /* Precondition: pointers must be non-null */
    __CPROVER_assume(a != NULL);
    __CPROVER_assume(b != NULL);

    /* Make valid aws_byte_buf structures using the proof helper */
    ensure_byte_buf_has_allocated_buffer_member(a);
    ensure_byte_buf_has_allocated_buffer_member(b);

    /* Precondition: both buffers must satisfy the validity predicate */
    __CPROVER_assume(aws_byte_buf_is_valid(a));
    __CPROVER_assume(aws_byte_buf_is_valid(b));

    /* Save state before the call for frame condition checks */
    size_t a_len_before = a->len;
    size_t a_capacity_before = a->capacity;
    struct aws_allocator *a_allocator_before = a->allocator;
    uint8_t *a_buffer_before = a->buffer;

    size_t b_len_before = b->len;
    size_t b_capacity_before = b->capacity;
    struct aws_allocator *b_allocator_before = b->allocator;
    uint8_t *b_buffer_before = b->buffer;

    /* Call the function under verification */
    bool result = aws_byte_buf_eq(a, b);

    /* Postcondition: both buffers remain valid after the call */
    assert(aws_byte_buf_is_valid(a));
    assert(aws_byte_buf_is_valid(b));

    /* Postcondition: frame conditions - struct fields not modified */
    assert(a->len == a_len_before);
    assert(a->capacity == a_capacity_before);
    assert(a->allocator == a_allocator_before);
    assert(a->buffer == a_buffer_before);

    assert(b->len == b_len_before);
    assert(b->capacity == b_capacity_before);
    assert(b->allocator == b_allocator_before);
    assert(b->buffer == b_buffer_before);

    /* Postcondition: if lengths differ, result must be false */
    if (a->len != b->len) {
        assert(result == false);
    }

    /* Postcondition: if both lengths are zero, result must be true */
    if (a->len == 0 && b->len == 0) {
        assert(result == true);
    }

    /* Postcondition: result is consistent with aws_array_eq semantics */
    /* If result is true, lengths must be equal */
    if (result == true) {
        assert(a->len == b->len);
    }
}
