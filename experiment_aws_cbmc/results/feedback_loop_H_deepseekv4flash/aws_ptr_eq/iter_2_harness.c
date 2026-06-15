#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ptr_eq_harness() {
    const void *a = nondet_voidp();
    const void *b = nondet_voidp();

    bool result = aws_ptr_eq(a, b);

    assert(result == (a == b));
}

int main() {
    aws_ptr_eq_harness();
    return 0;
}
