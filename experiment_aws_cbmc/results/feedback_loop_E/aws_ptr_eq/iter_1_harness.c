#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ptr_eq_harness() {
    const void *a = (const void *)nondet_size_t();
    const void *b = (const void *)nondet_size_t();

    bool result = aws_ptr_eq(a, b);

    // Postconditions
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
