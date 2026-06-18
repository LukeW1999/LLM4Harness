#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *str = ensure_string_is_allocated(MAX_STRING_SIZE);
    if (str == NULL) {
        return;
    }
    struct aws_byte_cursor result = aws_byte_cursor_from_string(str);
    assert(result.ptr == str->bytes);
    assert(result.len == str->len);
    assert(aws_byte_cursor_is_valid(&result));
}
