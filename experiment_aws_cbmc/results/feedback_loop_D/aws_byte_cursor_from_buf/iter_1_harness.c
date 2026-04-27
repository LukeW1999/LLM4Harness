// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_from_buf returns AWS_OP_SUCCESS (or the successful value):
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//   - cur.ptr: CHANGES to buf->buffer
//   - cur.len: CHANGES to buf->len
//
// === STEP 2: FAILURE PATH ===
// There is no failure path for this function as it does not return an error code.
// The function always succeeds and returns a struct aws_byte_cursor.
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//   - cur.ptr: CHANGES to buf->buffer
//   - cur.len: CHANGES to buf->len
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: UNCHANGED
//     - len: UNCHANGED
//     - capacity: UNCHANGED
//     - allocator: UNCHANGED
//   cur (struct aws_byte_cursor):
//     - ptr: CHANGED
//     - len: CHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)
//   - aws_byte_cursor_is_valid(&cur): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <assert.h>

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf = buf;
    buf.buffer = (uint8_t *)malloc(buf.capacity * sizeof(uint8_t));
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));

    ensure_byte_buf_has_allocated_buffer_member(&buf);

    struct aws_byte_cursor cur;

    cur = aws_byte_cursor_from_buf(&buf);

    // Assert frame conditions
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    // Assert validity invariants
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));

    free(buf.buffer);
    free(buf.allocator);
}
