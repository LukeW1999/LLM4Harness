#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness() {
    struct aws_string str;
    __CPROVER_assume(aws_string_is_valid(&str));
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)&str);

    const uint8_t *result = aws_string_bytes(&str);

    assert(result == str.bytes);
    assert(str.allocator == str.allocator);
    assert(str.len == str.len);
    assert(aws_string_is_valid(&str));
}

int main() {
    aws_string_bytes_harness();
    return 0;
}
