// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_write_u8 returns true (success):
//   - buf->len: CHANGES to old_len + 1
//   - buf->buffer[old_len]: CHANGES to c (the byte written)
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_write_u8 returns false (failure):
//   - buf->len: UNCHANGED
//   - buf->buffer: UNCHANGED (contents unchanged)
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   buf (struct aws_byte_buf):
//     - buffer: UNCHANGED (pointer itself), content at old_len CHANGED on success
//     - len: CHANGED on success (old_len + 1), UNCHANGED on failure
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_write_u8_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Non-deterministic byte value to write */
    uint8_t c = nondet_uint8_t();

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_byte_buf_write_u8(&buf, c);

    /* === STEP 4: Validity invariant === */
    assert(aws_byte_buf_is_valid(&buf));

    /* === STEP 3: Frame conditions - always unchanged === */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    if (result) {
        /* === STEP 1: Success path === */
        /* len increased by 1 */
        assert(buf.len == old_buf.len + 1);
        /* the byte was written at old position */
        assert(buf.buffer[old_buf.len] == c);
    } else {
        /* === STEP 2: Failure path === */
        /* len unchanged */
        assert(buf.len == old_buf.len);
    }
}
