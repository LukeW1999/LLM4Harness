#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_harness() {
    struct aws_string a = nondet_string();
    struct aws_string b = nondet_string();
    __CPROVER_assume(aws_string_is_valid(&a));
    __CPROVER_assume(aws_string_is_valid(&b));

    struct aws_string old_a = a;
    struct aws_string old_b = b;

    bool result = aws_string_eq(&a, &b);

    assert(result == (a.len == b.len && aws_array_eq(a.bytes, a.len, b.bytes, b.len)));

    assert(a.allocator == old_a.allocator);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(b.allocator == old_b.allocator);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);

    assert(aws_string_is_valid(&a));
    assert(aws_string_is_valid(&b));
}

int main() {
    aws_string_eq_harness();
    return 0;
}
