#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_bytes_harness() {
    struct aws_string str;
    struct aws_byte_buf buf = ensure_byte_buf_is_bounded_length(
        aws_default_allocator(), MAX_STRING_LEN);
    str = aws_string_new_from_array(buf.buffer, buf.len);
    __CPROVER_assume(aws_string_is_valid(&str));

    const uint8_t *result = aws_string_bytes(&str);

    assert(result == str.bytes);
    assert(str.len == str.len);
    assert(aws_string_is_valid(&str));
}

int main() {
    aws_string_bytes_harness();
    return 0;
}
