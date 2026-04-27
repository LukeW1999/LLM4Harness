// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_write_u8 calls aws_byte_buf_write(buf, &c, 1)
// aws_byte_buf_write succeeds when:
//   - len == 0 (trivially true, but len=1 here so this doesn't apply)
//   - buf->len <= (SIZE_MAX >> 1) && 1 <= (SIZE_MAX >> 1) && buf->len + 1 <= buf->capacity
// On success:
//   - buf->buffer[buf->len] = c (the byte is written)
//   - buf->len: CHANGES to old_len + 1
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//   - buf->buffer ptr: UNCHANGED (same pointer, content at old_len changes)
//
// === STEP 2: FAILURE PATH ===
// aws_byte_buf_write fails when:
//   - buf->len > (SIZE_MAX >> 1), OR
//   - 1 > (SIZE_MAX >> 1) (impossible), OR
//   - buf->len + 1 > buf->capacity
// On failure:
//   - buf->len: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//   - buf->buffer: UNCHANGED (pointer and contents)
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buffer ptr: UNCHANGED always (same pointer)
//   - buffer[old_len]: CHANGED on success (written with c), UNCHANGED on failure
//   - len: CHANGED on success (old_len + 1), UNCHANGED on failure
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// c (uint8_t, value parameter): N/A - not a struct
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold after call, both success and failure)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_write_u8_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Non-deterministic byte value to write */
    uint8_t c = nondet_uint8_t();

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_byte_buf_write_u8(&buf, c);

    /* === STEP 4: Validity invariant must hold after call === */
    assert(aws_byte_buf_is_valid(&buf));

    /* === STEP 3: Frame conditions - always unchanged === */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    if (result) {
        /* === STEP 1: Success path === */
        /* buf->len increased by 1 */
        assert(buf.len == old_buf.len + 1);
        /* The byte was written at old position */
        assert(buf.buffer[old_buf.len] == c);
    } else {
        /* === STEP 2: Failure path === */
        /* buf->len unchanged */
        assert(buf.len == old_buf.len);
        /* Failure condition: buf->len > (SIZE_MAX >> 1) OR buf->len + 1 > buf->capacity */
        assert(old_buf.len > (SIZE_MAX >> 1) || old_buf.len + 1 > old_buf.capacity);
    }
}
