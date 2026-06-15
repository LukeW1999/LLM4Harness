#include <aws/common/hash_table.h>

void aws_ptr_eq_harness() {
    const void *a;
    const void *b;

    // Use nondeterministic branch to ensure both equality and inequality are covered
    if (__CPROVER_bool) {
        __CPROVER_assume(a == b);
    } else {
        __CPROVER_assume(a != b);
    }

    bool result = aws_ptr_eq(a, b);
    __CPROVER_assert(result == (a == b), "aws_ptr_eq returns true iff pointers are equal");
}

int main() {
    aws_ptr_eq_harness();
    return 0;
}
