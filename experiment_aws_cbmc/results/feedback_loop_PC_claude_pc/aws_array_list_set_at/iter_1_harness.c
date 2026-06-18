#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Non-deterministic index */
    size_t index;
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Allocate a val buffer of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must never change */
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data pointer must be non-null */
        assert(list.data != NULL);

        /* On success: current_size must be >= (index+1)*item_size */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* On success: length must be > index */
        assert(list.length > index);

        /* If index was >= old length, length should be index+1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within existing bounds */
            assert(list.length == old_list.length);
        }

        /* The data at index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

        /* current_size must be >= old current_size (can only grow) */
        assert(list.current_size >= old_list.current_size);

    } else {
        /* On failure: the list state should be consistent */
        /* current_size and length may or may not have changed depending on
           where the failure occurred; but validity must hold */
        assert(aws_array_list_is_valid(&list));

        /* item_size and alloc must be unchanged */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }
}
