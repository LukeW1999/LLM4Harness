#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    struct aws_string str;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_string_is_valid(&str));
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)&str);
    __CPROVER_assume(str.allocator == allocator);
    __CPROVER_assume(str.len > 0);

    struct aws_string old = str;

    aws_string_destroy_secure(&str);

    assert(str.allocator == NULL);
    assert(str.len == 0);
    assert(str.capacity == 0);
    assert(str.buffer == NULL);

    assert(old.allocator!= NULL);
    assert(old.len > 0);
}

int main() {
    aws_string_destroy_secure_harness();
    return 0;
}
