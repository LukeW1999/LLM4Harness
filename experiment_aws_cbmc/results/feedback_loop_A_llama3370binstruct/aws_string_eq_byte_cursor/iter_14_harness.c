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
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_string_is_valid(&str));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    bool result = aws_string_eq_byte_cursor(&str, &cur);

    assert(result == aws_array_eq(str.bytes, str.len, cur.ptr, cur.len));

    struct aws_string old_str = str;
    struct aws_byte_cursor old_cur = cur;

    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);

    assert(str.len == 0 || str.bytes != NULL);
    assert(cur.len == 0 || cur.ptr != NULL);
    assert(str.len == 0 || str.capacity >= str.len);
    assert(cur.len == 0 || cur.len <= str.capacity);
    assert(str.allocator == allocator);
}

int main() {
    aws_string_eq_byte_cursor_harness();
    return 0;
}
