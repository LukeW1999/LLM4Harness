// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_clean_up returns AWS_OP_SUCCESS (or the successful value):
//   - buf->buffer: CHANGES to NULL
//   - buf->len: CHANGES to 0
//   - buf->capacity: CHANGES to 0
//   - buf->allocator: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_clean_up returns AWS_OP_ERR (or fails):
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)
```

```c
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf = buf;

    // Ensure buf is initially valid
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    aws_byte_buf_clean_up(&buf);

    // Success path assertions
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    // Frame condition assertions
    if (aws_byte_buf_is_valid(&old_buf) && old_buf.allocator != NULL) {
        // If the original buffer was valid and had a non-null allocator, it should have been cleaned up successfully
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    } else {
        // If the original buffer was invalid or had a null allocator, it should remain unchanged
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    // Validity invariant assertions
    assert(aws_byte_buf_is_valid(&buf));
}
