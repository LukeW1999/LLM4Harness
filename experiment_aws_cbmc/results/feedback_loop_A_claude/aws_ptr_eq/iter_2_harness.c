#include <aws/common/hash_table.h>
#include <aws/common/common.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

void aws_ptr_eq_harness(void) {
    /* 1. Declare inputs - two arbitrary pointers */
    const void *a;
    const void *b;

    /* 2. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 3. Assert postconditions */
    /* The function compares pointer equality */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Equivalently, the result must exactly match pointer equality */
    assert(result == (a == b));
}
