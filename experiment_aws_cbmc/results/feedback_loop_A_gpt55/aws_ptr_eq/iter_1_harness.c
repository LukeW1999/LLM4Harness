#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness(void) {
    uint8_t object_a;
    uint8_t object_b;

    const void *a;
    const void *b;

    if (nondet_bool()) {
        a = NULL;
    } else if (nondet_bool()) {
        a = &object_a;
    } else {
        a = &object_b;
    }

    if (nondet_bool()) {
        b = NULL;
    } else if (nondet_bool()) {
        b = &object_a;
    } else {
        b = &object_b;
    }

    const void *old_a = a;
    const void *old_b = b;

    bool result = aws_ptr_eq(a, b);

    assert(a == old_a);
    assert(b == old_b);

    assert(result == (old_a == old_b));

    if (result) {
        assert(old_a == old_b);
    } else {
        assert(old_a != old_b);
    }
}
