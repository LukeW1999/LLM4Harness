// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_eq_c_str returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When it returns true:
//   - buf->len: UNCHANGED
//   - buf->buffer: UNCHANGED (pointer)
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//   - c_str: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When it returns false:
//   - buf->len: UNCHANGED
//   - buf->buffer: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//   - c_str: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - len: UNCHANGED always (read-only comparison)
//   - buffer: UNCHANGED always (read-only comparison)
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
// c_str (const char *):
//   - contents: UNCHANGED always (read-only comparison)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold before and after call)
//   - The function only reads from buf and c_str, no modifications

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 8
#define MAX_STRING_LEN 8

void aws_byte_buf_eq_c_str_harness(void) {
    /* Set up aws_byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Set up c_str - can be NULL or a valid string */
    /* We use a bounded array to keep verification tractable */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STRING_LEN);
    char *c_str = malloc(str_len + 1);
    if (c_str != NULL) {
        /* Make it a valid null-terminated string */
        c_str[str_len] = '\0';
        /* Contents are nondet (already nondet from malloc in CBMC) */
    }
    /* c_str may be NULL if buf.len == 0, but aws_array_eq_c_str handles NULL c_str
       only when array_len is 0. Let's allow NULL c_str only when buf.len == 0 */
    /* Actually, looking at the spec: NULL may be passed as array pointer if length is 0.
       For c_str, it must be a valid null-terminated string or the behavior is undefined.
       We'll keep c_str non-NULL for safety, but allow it to be NULL when buf.len == 0 */

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* === ASSERTIONS === */

    /* Frame conditions: buf must be unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariant: buf must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Result type check: result is a bool */
    assert(result == true || result == false);

    /* Semantic check: if buf.len == 0 and c_str is non-NULL and c_str[0] == '\0',
       then result should be true */
    if (c_str != NULL && buf.len == 0 && c_str[0] == '\0') {
        assert(result == true);
    }

    /* Semantic check: if buf.len == 0 and c_str is non-NULL and c_str[0] != '\0',
       then result should be false */
    if (c_str != NULL && buf.len == 0 && c_str[0] != '\0') {
        assert(result == false);
    }
}
