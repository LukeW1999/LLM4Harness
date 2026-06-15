#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string src;
    __CPROVER_assume(aws_string_is_valid(&src));
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)&src);
    __CPROVER_assume(src.len > 0);
    __CPROVER_assume(src.bytes!= NULL);
    __CPROVER_assume(src.len <= SIZE_MAX);
    src.allocator = aws_default_allocator();
    __CPROVER_assume(src.allocator!= NULL);

    struct aws_string old_src = src;

    struct aws_byte_cursor result;
    aws_byte_cursor_from_string(&src, &result);

    assert(result.len == src.len);
    assert(result.ptr == src.bytes);

    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);

    assert(src.bytes!= NULL);
    assert(src.len <= SIZE_MAX);
    assert(aws_byte_cursor_is_valid(&result));
}

int main() {
    aws_byte_cursor_from_string_harness();
    return 0;
}
