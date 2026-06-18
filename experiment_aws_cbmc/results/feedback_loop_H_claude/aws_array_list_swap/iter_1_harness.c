#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Need at least 1 element to have valid indices */
    __CPROVER_assume(list.length > 0);

    /* 3. Non-deterministic indices within bounds */
    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* Save bytes at positions a and b before the swap */
    /* We need to capture the data at indices a and b */
    /* item_size > 0 is guaranteed by aws_array_list_is_valid */

    /* 5. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Assert postconditions */

    /* Structural fields must not change */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* 7. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* 8. If a == b, data is unchanged entirely */
    if (a == b) {
        /* The function returns early, data is unchanged */
        assert_bytes_match(
            (const uint8_t *)list.data,
            (const uint8_t *)old_list.data,
            list.current_size);
    }
    /* If a != b, elements at positions a and b are swapped.
     * We verify this by checking that the bytes at index a in the new list
     * match the bytes at index b in the old list, and vice versa.
     * This is done via assert_bytes_match on the relevant memory regions. */
    else {
        uint8_t *new_data = (uint8_t *)list.data;
        uint8_t *old_data = (uint8_t *)old_list.data;

        /* Element at index a in new list == element at index b in old list */
        assert_bytes_match(
            new_data + a * list.item_size,
            old_data + b * list.item_size,
            list.item_size);

        /* Element at index b in new list == element at index a in old list */
        assert_bytes_match(
            new_data + b * list.item_size,
            old_data + a * list.item_size,
            list.item_size);
    }
}
