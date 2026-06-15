#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_string_is_valid(&str));
    struct aws_byte_cursor cur;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cur);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_str = str;
    struct aws_byte_cursor old_cur = cur;

    /* 3. Call function under test */
    __CPROVER_assume(str.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(str.bytes!= NULL);
    __CPROVER_assume(cur.ptr!= NULL);
    __CPROVER_assume(str.len > 0 || cur.len == 0);
    __CPROVER_assume(cur.len > 0 || str.len == 0);
    __CPROVER_assume(str.bytes!= NULL && str.len > 0);
    __CPROVER_assume(cur.ptr!= NULL && cur.len > 0);
    __CPROVER_assume(str.len == 0 || str.bytes!= NULL);
    __CPROVER_assume(cur.len == 0 || cur.ptr!= NULL);
    __CPROVER_assume(str.len == 0 || aws_is_mem_region_valid(str.bytes, str.len));
    __CPROVER_assume(cur.len == 0 || aws_is_mem_region_valid(cur.ptr, cur.len));
    __CPROVER_assume(str.allocator == allocator);
    __CPROVER_assume(str.len == 0 || str.bytes!= NULL);
    __CPROVER_assume(cur.len == 0 || cur.ptr!= NULL);
    __CPROVER_assume(str.len == 0 || aws_is_mem_region_valid(str.bytes, str.len));
    __CPROVER_assume(cur.len == 0 || aws_is_mem_region_valid(cur.ptr, cur.len));
    bool result = aws_string_eq_byte_cursor(&str, &cur);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(aws_array_eq(str.bytes, str.len, cur.ptr, cur.len));
    } else {
        assert(!aws_array_eq(str.bytes, str.len, cur.ptr, cur.len));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);

    /* 6. Assert validity invariant always holds */
    assert(str.len == 0 || str.bytes!= NULL);
    assert(cur.len == 0 || cur.ptr!= NULL);
    assert(str.len == 0 || aws_is_mem_region_valid(str.bytes, str.len));
    assert(cur.len == 0 || aws_is_mem_region_valid(cur.ptr, cur.len));
    assert(str.allocator == allocator);
}
