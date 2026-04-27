// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_from_buf always succeeds (no error return).
// It returns a new aws_byte_cursor where:
//   - result.ptr = buf->buffer
//   - result.len = buf->len
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns a cursor.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf, input only):
//   - buf->len: UNCHANGED always
//   - buf->buffer: UNCHANGED always
//   - buf->capacity: UNCHANGED always
//   - buf->allocator: UNCHANGED always
// result (struct aws_byte_cursor, returned by value):
//   - result.ptr: SET to buf->buffer
//   - result.len: SET to buf->len
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold before call)
//   - aws_byte_cursor_is_valid(&result): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_buf(&buf);

    /* Step 1: Verify return value fields */
    assert(result.ptr == buf.buffer);
    assert(result.len == buf.len);

    /* Step 3: Verify frame conditions - buf is unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Step 4: Verify validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&result));
}
