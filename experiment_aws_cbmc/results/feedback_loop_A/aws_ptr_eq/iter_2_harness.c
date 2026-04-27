#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    /* 1. Declare and bound data structures */
    const void *a = (const void *)nondet_void_ptr();
    const void *b = (const void *)nondet_void_ptr();

    /* 2. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
