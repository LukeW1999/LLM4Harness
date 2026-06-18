#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    /* Declare non-deterministic pointers */
    const void *a;
    const void *b;

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: the result is true exactly when the pointers are equal */
    assert(result == (a == b));
}
