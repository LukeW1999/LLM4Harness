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
    uint8_t obj_a = nondet_uint8_t();
    uint8_t obj_b = nondet_uint8_t();

    uint8_t old_obj_a = obj_a;
    uint8_t old_obj_b = obj_b;

    const void *a;
    const void *b;

    switch (nondet_uint8_t() % 3) {
        case 0:
            a = NULL;
            break;
        case 1:
            a = &obj_a;
            break;
        default:
            a = &obj_b;
            break;
    }

    switch (nondet_uint8_t() % 3) {
        case 0:
            b = NULL;
            break;
        case 1:
            b = &obj_a;
            break;
        default:
            b = &obj_b;
            break;
    }

    const void *old_a = a;
    const void *old_b = b;

    bool result = aws_ptr_eq(a, b);

    assert(result == (a == b));

    if (result) {
        assert(a == b);
    } else {
        assert(a != b);
    }

    assert(a == old_a);
    assert(b == old_b);
    assert(obj_a == old_obj_a);
    assert(obj_b == old_obj_b);
}
