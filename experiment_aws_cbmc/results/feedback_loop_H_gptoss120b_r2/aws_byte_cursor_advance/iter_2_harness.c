#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;

    /* cursor must be bounded and have a concrete buffer */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* nondet amount to advance, bounded */
    size_t amount;
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    /* save old state */
    struct aws_byte_cursor old = cursor;

    /* call function under test */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(cursor.ptr == old.ptr + amount);
        assert(cursor.len == old.len - amount);
    } else {
        assert(cursor.ptr == old.ptr);
        assert(cursor.len == old.len);
    }

    /* invariants must hold after the call */
    assert(aws_byte_cursor_is_valid(&cursor));
}
