#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    uint8_t object_a;
    uint8_t object_b;

    const void *a;
    const void *b;

    uint8_t a_choice = nondet_uint8_t();
    uint8_t b_choice = nondet_uint8_t();

    __CPROVER_assume(a_choice < 3);
    __CPROVER_assume(b_choice < 3);

    if (a_choice == 0) {
        a = NULL;
    } else if (a_choice == 1) {
        a = &object_a;
    } else {
        a = &object_b;
    }

    if (b_choice == 0) {
        b = NULL;
    } else if (b_choice == 1) {
        b = &object_a;
    } else {
        b = &object_b;
    }

    const void *old_a = a;
    const void *old_b = b;
    uint8_t old_object_a = object_a;
    uint8_t old_object_b = object_b;

    bool result = aws_ptr_eq(a, b);

    if (result) {
        assert(a == b);
    } else {
        assert(a != b);
    }

    assert(result == (a == b));

    assert(a == old_a);
    assert(b == old_b);
    assert(object_a == old_object_a);
    assert(object_b == old_object_b);

    assert(a == NULL || AWS_MEM_IS_READABLE(a, sizeof(uint8_t)));
    assert(b == NULL || AWS_MEM_IS_READABLE(b, sizeof(uint8_t)));
}
