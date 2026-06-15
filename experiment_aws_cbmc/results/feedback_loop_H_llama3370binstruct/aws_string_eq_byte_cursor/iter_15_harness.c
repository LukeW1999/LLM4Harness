#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string str;
    struct aws_byte_cursor cur;
    __CPROVER_assume(aws_string_is_valid(&str));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cur);

    str.allocator = aws_default_allocator();
    cur.allocator = aws_default_allocator();

    bool result = aws_string_eq_byte_cursor(&str, &cur);

    assert(aws_string_is_valid(&str));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    if (result) {
        assert(str.len == cur.len);
        assert(memcmp(str.bytes, cur.ptr, str.len) == 0);
    } else {
        assert(str.len != cur.len || memcmp(str.bytes, cur.ptr, str.len) != 0);
    }
}

int main() {
    aws_string_eq_byte_cursor_harness();
    return 0;
}
