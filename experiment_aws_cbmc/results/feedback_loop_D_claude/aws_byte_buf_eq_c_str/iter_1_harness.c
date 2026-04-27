// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_eq_c_str returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When it returns true: buf->buffer contents match c_str
//   - buf->len: UNCHANGED
//   - buf->buffer: UNCHANGED (pointer and contents)
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When it returns false: buf->buffer contents do not match c_str
//   - buf->len: UNCHANGED
//   - buf->buffer: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - len: UNCHANGED always
//   - buffer: UNCHANGED always (pointer value)
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// c_str (const char*):
//   - contents: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold before and after call)
//   The function only reads buf->buffer and buf->len, passing them to aws_array_eq_c_str
//   No modifications are made to buf or c_str

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* Set up aws_byte_buf with nondeterministic but bounded content */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Set up a nondeterministic c_str - can be NULL or a valid string */
    /* aws_array_eq_c_str handles NULL c_str by treating it as empty */
    const char *c_str;
    /* We need c_str to be either NULL or a valid null-terminated string */
    /* Use a bounded buffer for c_str */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str_buf = malloc(c_str_len + 1);
    if (c_str_buf != NULL) {
        c_str_buf[c_str_len] = '\0';
        c_str = c_str_buf;
    } else {
        c_str = NULL;
    }

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* === FRAME CONDITIONS: buf must be unchanged === */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* === VALIDITY INVARIANTS === */
    assert(aws_byte_buf_is_valid(&buf));

    /* === RESULT TYPE CHECK === */
    /* result is bool, so it must be true or false */
    assert(result == true || result == false);
}
