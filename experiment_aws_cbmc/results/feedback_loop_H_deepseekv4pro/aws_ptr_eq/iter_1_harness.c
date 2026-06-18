#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    /* 1. Declare and bound inputs */
    void *a;
    void *b;

    /* 2. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 3. Assert postconditions */
    /* aws_ptr_eq returns true iff a == b (pointer equality) */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* 4. No state was modified — nothing to check for immutability */
}
