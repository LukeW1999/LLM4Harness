#include <aws/common/byte_buf.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <limits.h>

void aws_nospec_mask_harness() {
    size_t index;
    size_t bound;

    index = nondet_size_t();
    bound = nondet_size_t();

    // Ensure both branches are reachable
    bool branch_case = nondet_bool();
    if (branch_case) {
        __CPROVER_assume(index < bound);
    } else {
        __CPROVER_assume(index >= bound);
    }

    size_t result = aws_nospec_mask(index, bound);

    // Postcondition: result is either 0 or UINTPTR_MAX
    assert(result == 0 || result == UINTPTR_MAX);

    // If index >= bound, result must be 0
    if (index >= bound) {
        assert(result == 0);
    }

    // If index < bound, result must be UINTPTR_MAX
    if (index < bound) {
        assert(result == UINTPTR_MAX);
    }
}
