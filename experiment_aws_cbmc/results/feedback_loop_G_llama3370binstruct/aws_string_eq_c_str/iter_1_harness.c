#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string str;
    __CPROVER_assume(aws_string_is_valid(&str));
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)&str);

    const char *c_str;
    __CPROVER_assume(c_str!= NULL);

    bool result = aws_string_eq_c_str(&str, c_str);

    if (result) {
        assert(aws_array_eq_c_str(str.bytes, str.len, c_str));
    } else {
        assert(!aws_array_eq_c_str(str.bytes, str.len, c_str));
    }

    assert(str.allocator == str.allocator);
    assert(str.len == str.len);

    assert(aws_string_is_valid(&str));
}

int main() {
    aws_string_eq_c_str_harness();
    return 0;
}
