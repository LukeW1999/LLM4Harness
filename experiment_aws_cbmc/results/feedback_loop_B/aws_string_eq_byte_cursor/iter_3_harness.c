#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_byte_cursor_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    struct aws_byte_cursor cur;
    __CPROVER_assume(aws_string_is_bounded(&str, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_string_is_valid(&str));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_str = str;
    struct aws_byte_cursor old_cur = cur;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(&str, &cur);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (str.len == cur.len && __CPROVER_forall { size_t i; (i < str.len ==> str.bytes[i] == cur.ptr[i]) }) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(cur.allocator == old_cur.allocator);
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str));
    assert(aws_byte_cursor_is_valid(&cur));
}
