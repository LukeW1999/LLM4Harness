#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src = ensure_string_is_bounded_length(
        1024, aws_default_allocator());
    __CPROVER_assume(aws_string_is_valid(src));

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    assert(cursor.len == src->len);
    assert(cursor.ptr == src->bytes);

    assert(aws_byte_cursor_is_valid(&cursor));
}

int main() {
    aws_byte_cursor_from_string_harness();
    return 0;
}
