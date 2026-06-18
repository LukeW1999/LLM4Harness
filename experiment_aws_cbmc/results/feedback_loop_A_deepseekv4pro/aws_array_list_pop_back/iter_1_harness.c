#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structure */
    struct aws_array_list list;
    struct aws_array_list old;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    old = list;

    /* 3. Prepare input value (size = list.item_size, filled nondeterministically) */
    size_t item_size = list.item_size;
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_r_ok(val, item_size));
    for (size_t i = 0; i < item_size; i++) {
        val[i] = nondet_uint8_t();
    }

    /* 4. Nondeterministic index; avoid overflow when computing index+1 */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < SIZE_MAX);

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: element at index must match val */
        assert_bytes_match((const uint8_t *)list.data + index * list.item_size,
                           val,
                           list.item_size);
        /* Length must be updated if index was beyond old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        /* Fields that must NOT change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* list.data and list.current_size are allowed to change (dynamic growth) */
    } else {
        /* Failure: the entire list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
    }

    /* 7. Validity invariant MUST hold in both cases */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
