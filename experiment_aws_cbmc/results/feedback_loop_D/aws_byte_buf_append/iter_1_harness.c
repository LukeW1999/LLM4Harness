// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_append returns AWS_OP_SUCCESS (or the successful value):
//   - to.len: CHANGES to to.len + from.len
//   - to.buffer: CHANGES to to.buffer + from.len (only if from.len > 0)
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_append returns AWS_OP_ERR (or fails):
//   - to.len: UNCHANGED
//   - to.buffer: UNCHANGED
//   - to.capacity: UNCHANGED
//   - to.allocator: UNCHANGED
//   - from.len: UNCHANGED
//   - from.ptr: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// to (struct aws_byte_buf):
//   - len: CHANGED on success, UNCHANGED on failure
//   - buffer: CHANGED on success (if from.len > 0), UNCHANGED on failure
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// from (struct aws_byte_cursor):
//   - len: UNCHANGED always
//   - ptr: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&to): YES (must hold after call)
//   - aws_byte_cursor_is_valid(&from): YES
```

#include <aws/common/byte_buf.h>
#include <aws/common/byte_order.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/proof_allocators.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    // Initialize to with arbitrary values
    to.len = nondet_size_t();
    to.capacity = nondet_size_t();
    to.allocator = (struct aws_allocator *)nondet_ptr();
    if (nondet_bool()) {
        to.buffer = bounded_malloc(to.capacity);
    } else {
        to.buffer = NULL;
    }

    // Initialize from with arbitrary values
    from.len = nondet_size_t();
    if (nondet_bool()) {
        from.ptr = bounded_malloc(from.len);
    } else {
        from.ptr = NULL;
    }

    // Ensure preconditions
    assume(aws_byte_buf_is_valid(&to));
    assume(aws_byte_cursor_is_valid(&from));

    int result = aws_byte_buf_append(&to, &from);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        if (from.len > 0) {
            assert_byte_from_buffer_matches(to.buffer + old_to.len, from.ptr, from.len);
        }
    } else {
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
    }

    // Frame conditions
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    // Validity invariants
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    // Free allocated memory
    free(to.buffer);
    free(from.ptr);
}
