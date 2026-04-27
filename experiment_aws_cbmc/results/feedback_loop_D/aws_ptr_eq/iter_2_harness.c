#include <aws/common/common.h>
#include <assert.h>

void aws_ptr_eq_harness() {
    const void *a = (const void *)nondet_ptr();
    const void *b = (const void *)nondet_ptr();

    // Save old state (not necessary for void pointers, but for completeness)
    const void *old_a = a;
    const void *old_b = b;

    bool result = aws_ptr_eq(a, b);

    // Frame conditions
    assert(a == old_a);
    assert(b == old_b);

    // Postconditions
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
