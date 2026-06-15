#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

static bool buffers_equal(const struct aws_byte_buf *a, const struct aws_byte_buf *b) {
    if (a->len != b->len) {
        return false;
    }
    for (size_t i = 0; i < a->len; ++i) {
        if (a->buffer[i] != b->buffer[i]) {
            return false;
        }
    }
    return true;
}

void aws_byte_buf_eq_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf a;
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* 3. Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* 4. Assert postconditions */
    assert(result == buffers_equal(&a, &b));

    /* 5. Assert fields that must NOT change regardless of result */
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.buffer == old_a.buffer);
    assert(a.allocator == old_a.allocator);

    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.buffer == old_b.buffer);
    assert(b.allocator == old_b.allocator);

    /* 6. Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
