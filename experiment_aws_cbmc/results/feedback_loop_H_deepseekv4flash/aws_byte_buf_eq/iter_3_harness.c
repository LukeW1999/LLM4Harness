#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf *a = malloc(sizeof(*a));
    struct aws_byte_buf *b = malloc(sizeof(*b));
    __CPROVER_assume(a != NULL && b != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(a);
    ensure_byte_buf_has_allocated_buffer_member(b);
    __CPROVER_assume(aws_byte_buf_is_valid(a));
    __CPROVER_assume(aws_byte_buf_is_valid(b));

    size_t old_a_len = a->len;
    size_t old_b_len = b->len;
    size_t old_a_cap = a->capacity;
    size_t old_b_cap = b->capacity;
    void *old_a_alloc = a->allocator;
    void *old_b_alloc = b->allocator;

    bool result = aws_byte_buf_eq(a, b);

    assert(a->len == old_a_len);
    assert(b->len == old_b_len);
    assert(a->capacity == old_a_cap);
    assert(b->capacity == old_b_cap);
    assert(a->allocator == old_a_alloc);
    assert(b->allocator == old_b_alloc);

    if (result) {
        assert(a->len == b->len);
    }

    free(a);
    free(b);
}
