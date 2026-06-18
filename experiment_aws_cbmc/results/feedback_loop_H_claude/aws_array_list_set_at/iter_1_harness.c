#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create val: a readable buffer of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Pick a non-deterministic index */
    size_t index;
    /* Bound index to keep state space manageable */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: data was written at index */
        /* The list must have data allocated */
        assert(list.data != NULL);

        /* item_size and alloc must not change */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* The value at index must match val */
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

        /* Length: if index >= old length, new length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* index < old length: length unchanged */
            assert(list.length == old_list.length);
        }

        /* current_size must be at least (index+1)*item_size */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: list fields should remain consistent */
        /* item_size and alloc must not change */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* Validity invariant still holds */
        assert(aws_array_list_is_valid(&list));
    }
}
