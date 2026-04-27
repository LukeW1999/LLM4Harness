// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_cat returns AWS_OP_SUCCESS:
//   - dest->len: CHANGES (increases by sum of all buffer lengths appended)
//   - dest->buffer: UNCHANGED (same pointer, but contents may change)
//   - dest->capacity: UNCHANGED
//   - dest->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_cat returns AWS_OP_ERR:
//   - dest->len: MAY HAVE CHANGED (partial appends may have occurred before failure)
//   - dest->buffer: UNCHANGED (same pointer)
//   - dest->capacity: UNCHANGED
//   - dest->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// dest (struct aws_byte_buf):
//   - len: CHANGED on success (increased), possibly changed on failure (partial)
//   - buffer: UNCHANGED always (same pointer)
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// buf1 (struct aws_byte_buf - vararg):
//   - len: UNCHANGED always
//   - buffer: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// buf2 (struct aws_byte_buf - vararg):
//   - len: UNCHANGED always
//   - buffer: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(dest): YES (must hold after call)
//   - aws_byte_buf_is_valid(buf1): YES (unchanged)
//   - aws_byte_buf_is_valid(buf2): YES (unchanged)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdarg.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 8

void aws_byte_buf_cat_harness() {
    /* Set up dest buffer */
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* Set up source buffers (varargs) */
    struct aws_byte_buf buf1;
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));

    struct aws_byte_buf buf2;
    ensure_byte_buf_has_allocated_buffer_member(&buf2);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));

    /* Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_buf1 = buf1;
    struct aws_byte_buf old_buf2 = buf2;

    /* Choose number of args: 0, 1, or 2 */
    size_t number_of_args;
    __CPROVER_assume(number_of_args <= 2);

    int result;
    if (number_of_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (number_of_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, &buf1);
    } else {
        result = aws_byte_buf_cat(&dest, 2, &buf1, &buf2);
    }

    /* Check return value is valid */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Frame conditions: dest capacity and allocator never change */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    /* On success, dest must be valid and len >= old len */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len >= old_dest.len);
    }

    /* On any path, dest must remain valid */
    assert(aws_byte_buf_is_valid(&dest));

    /* Source buffers must be unchanged */
    assert(buf1.len == old_buf1.len);
    assert(buf1.buffer == old_buf1.buffer);
    assert(buf1.capacity == old_buf1.capacity);
    assert(buf1.allocator == old_buf1.allocator);
    assert(aws_byte_buf_is_valid(&buf1));

    assert(buf2.len == old_buf2.len);
    assert(buf2.buffer == old_buf2.buffer);
    assert(buf2.capacity == old_buf2.capacity);
    assert(buf2.allocator == old_buf2.allocator);
    assert(aws_byte_buf_is_valid(&buf2));
}
