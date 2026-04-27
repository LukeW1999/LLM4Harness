// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_advance succeeds (len <= cursor->len and neither > SIZE_MAX>>1):
//   - rv.ptr: CHANGES to old cursor->ptr
//   - rv.len: CHANGES to len
//   - cursor->ptr: CHANGES to old cursor->ptr + len (or NULL if was NULL)
//   - cursor->len: CHANGES to old cursor->len - len
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_advance fails (len > cursor->len OR len > SIZE_MAX>>1 OR cursor->len > SIZE_MAX>>1):
//   - rv.ptr: NULL
//   - rv.len: 0
//   - cursor->ptr: UNCHANGED
//   - cursor->len: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   cursor (struct aws_byte_cursor):
//     - ptr: CHANGED on success (ptr + len), UNCHANGED on failure
//     - len: CHANGED on success (len - old_len), UNCHANGED on failure
//   rv (struct aws_byte_cursor, return value):
//     - ptr: old cursor->ptr on success, NULL on failure
//     - len: len on success, 0 on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(cursor): YES (postcondition)
//   - aws_byte_cursor_is_valid(&rv): YES (postcondition)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <assert.h>

void aws_byte_cursor_advance_harness(void) {
    /* Allocate and initialize a nondeterministic cursor */
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Nondeterministic advance length */
    size_t len = nondet_size_t();

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call the function under test */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, len);

    /* === STEP 4: Validity invariants === */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&rv));

    /* Determine success/failure condition */
    bool success = !(old_cursor.len > (SIZE_MAX >> 1)) &&
                   !(len > (SIZE_MAX >> 1)) &&
                   (len <= old_cursor.len);

    if (success) {
        /* === STEP 1: Success path assertions === */
        /* rv.ptr == old cursor->ptr */
        assert(rv.ptr == old_cursor.ptr);
        /* rv.len == len */
        assert(rv.len == len);
        /* cursor->len == old_cursor.len - len */
        assert(cursor.len == old_cursor.len - len);
        /* cursor->ptr == old_cursor.ptr + len (or NULL if was NULL) */
        if (old_cursor.ptr == NULL) {
            assert(cursor.ptr == NULL);
        } else {
            assert(cursor.ptr == old_cursor.ptr + len);
        }
    } else {
        /* === STEP 2: Failure path assertions === */
        /* rv.ptr == NULL */
        assert(rv.ptr == NULL);
        /* rv.len == 0 */
        assert(rv.len == 0);
        /* cursor unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }
}
