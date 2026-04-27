// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_from_buf returns a valid aws_byte_cursor:
//   - cur.ptr: CHANGES to buf->buffer
//   - cur.len: CHANGES to buf->len
//
// === STEP 2: FAILURE PATH ===
// aws_byte_cursor_from_buf does not return a status code; it always succeeds in creating a cursor.
// Therefore, there is no failure path to consider.
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: UNCHANGED always
//     - len: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//   cur (struct aws_byte_cursor):
//     - ptr: CHANGED always
//     - len: CHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)
//   - aws_byte_cursor_is_valid(&cur): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    // Frame conditions
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    // Success path conditions
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    // Validity invariants
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));
}
