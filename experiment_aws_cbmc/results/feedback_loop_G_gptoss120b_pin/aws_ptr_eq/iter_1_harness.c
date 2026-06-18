#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_ptr_eq_harness() {
    /* nondeterministic pointer values */
    const void *a = (const void *)nondet_uint64_t();
    const void *b = (const void *)nondet_uint64_t();

    /* call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* postcondition: result must reflect pointer equality */
    assert(result == (a == b));
}
