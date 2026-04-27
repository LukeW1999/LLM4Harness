// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_from_buf always returns a struct aws_byte_cursor (no error path).
// The returned cursor:
//   - cur.ptr = buf->buffer
//   - cur.len = buf->len
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always succeeds and returns a cursor.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (const struct aws_byte_buf *):
//   - buf->len: UNCHANGED always (input only)
//   - buf->buffer: UNCHANGED always (input only)
//   - buf->capacity: UNCHANGED always (input only)
//   - buf->allocator: UNCHANGED always (input only)
// returned cursor (struct aws_byte_cursor):
//   - cur.ptr: SET to buf->buffer
//   - cur.len: SET to buf->len
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold before call)
//   - aws_byte_cursor_is_valid(&result): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_buf_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;
    
    /* Set up non-deterministic fields */
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)nondet_size_t();
    
    /* Ensure buf is valid: len <= capacity, and buffer is allocated if capacity > 0 */
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    
    /* Precondition: buf must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Save old state */
    struct aws_byte_buf old_buf = buf;
    
    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_buf(&buf);
    
    /* === Assertions for Step 1: Return value correctness === */
    assert(result.ptr == old_buf.buffer);
    assert(result.len == old_buf.len);
    
    /* === Assertions for Step 3: Frame conditions on buf === */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    
    /* === Assertions for Step 4: Validity invariants === */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&result));
}
