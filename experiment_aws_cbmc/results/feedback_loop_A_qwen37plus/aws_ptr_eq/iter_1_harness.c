#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    const void *a = nondet_bool() ? (const void *)1 : NULL;
    const void *b = nondet_bool() ? (const void *)1 : NULL;

    bool result = aws_ptr_eq(a, b);

    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
