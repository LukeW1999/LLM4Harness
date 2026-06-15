#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_ptr_eq_harness() {
    /* Non-deterministic pointers */
    const void *a;
    const void *b;
    /* No need to assume anything about pointers; they can be NULL or any value */
    /* Call the function */
    bool result = aws_ptr_eq(a, b);
    /* Postcondition: result is true iff pointers are equal */
    assert(result == (a == b));
}
