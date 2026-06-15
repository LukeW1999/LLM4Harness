#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string str;
    struct aws_byte_cursor cur;
    __CPROVER_assume(aws_string_is_valid(&str));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cur);

    str.allocator = aws_default_allocator();
    cur.allocator = aws_default_allocator();

    struct aws_string old_str = str;
    struct aws_byte_cursor old_cur = cur;

    bool result = aws_string_eq_byte_cursor(&str, &cur);

    if (result) {
        assert(str.len == old_str.len);
        assert(str.allocator == old_str.allocator);
        assert(cur.len == old_cur.len);
        assert(cur.ptr == old_cur.ptr);
    } else {
        assert(str.len == old_str.len);
        assert(str.allocator == old_str.allocator);
        assert(cur.len == old_cur.len);
        assert(cur.ptr == old_cur.ptr);
    }

    assert(str.allocator == old_str.allocator);
    assert(cur.ptr == old_cur.ptr);

    assert(aws_string_is_valid(&str));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}

int main() {
    aws_string_eq_byte_cursor_harness();
    return 0;
}
