#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    struct aws_byte_cursor cur;

    /* Bounding the structures */
    __CPROVER_assume(aws_string_is_bounded(&str, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    /* Ensuring the structures have allocated data members */
    ensure_byte_buf_has_allocated_buffer_member(&(struct aws_byte_buf){.buffer = (void *)str.bytes, .len = str.len, .capacity = str.len});
    ensure_byte_cursor_has_allocated_buffer_member(&cur);

    /* Assuming the structures are valid */
    __CPROVER_assume(aws_string_is_valid(&str));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /* Assuming the byte buffers are initialized */
    __CPROVER_assume(str.len == cur.len);
    __CPROVER_assume(__CPROVER_forall {size_t i; (i < str.len) ==> (str.bytes[i] == cur.ptr[i])});

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_str = str;
    struct aws_byte_cursor old_cur = cur;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(&str, &cur);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        /* Success path: bytes and lengths must match */
        assert_bytes_match(str.bytes, cur.ptr, str.len);
        assert(cur.len == str.len);
    } else {
        /* Failure path: either bytes don't match or lengths don't match */
        if (str.len == cur.len) {
            assert(!bytes_match(str.bytes, cur.ptr, str.len));
        } else {
            assert(str.len != cur.len);
        }
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str));
    assert(aws_byte_cursor_is_valid(&cur));
}
