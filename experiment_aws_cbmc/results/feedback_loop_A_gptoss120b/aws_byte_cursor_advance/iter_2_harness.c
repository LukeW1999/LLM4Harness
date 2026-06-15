#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_cursor.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_CURSOR_LEN 256

void aws_byte_cursor_advance_harness(void) {
    /* 1. Declare inputs and bound them */
    struct aws_byte_cursor cursor;
    size_t amount = nondet_size_t();

    /* Allocate a buffer for the cursor and bound its size */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_CURSOR_LEN));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Bound the amount to avoid overflow in arithmetic */
    __CPROVER_assume(amount <= MAX_CURSOR_LEN);

    /* 2. Save old state of cursor */
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Call the function under test */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Advance succeeded: length decreased by amount and pointer advanced */
        assert(cursor.len == old_cursor.len - amount);
        assert(cursor.ptr == (const uint8_t *)old_cursor.ptr + amount);
    } else {
        /* Advance failed: cursor must remain unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* 5. Invariant: cursor remains valid regardless of outcome */
    assert(aws_byte_cursor_is_valid(&cursor));
}
