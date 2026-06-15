#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    /* 1. Declare and bound data structures */
    void *a = (void *)nondet_uint64_t();
    void *b = (void *)nondet_uint64_t();

    /* 2. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
