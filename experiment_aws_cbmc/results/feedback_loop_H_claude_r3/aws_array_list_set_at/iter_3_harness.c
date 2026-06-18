#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* Declare the array list */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* Use aws_default_allocator */
    list.alloc = aws_default_allocator();

    /* Create a val buffer of item_size bytes */
    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Non-deterministic index - keep it bounded */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Save old state for comparison */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */

    /* Validity invariant must always hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* item_size must never change */
    assert(list.item_size == item_size);

    /* alloc must never change */
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data pointer must be non-null */
        assert(list.data != NULL);

        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* current_size must accommodate at least index+1 items */
        assert(list.current_size >= (index + 1) * item_size);

        /* If index was already within old length, length stays the same */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            /* length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* The data at index should be readable */
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + (item_size * index), item_size));

        /* The data at index should match val */
        assert_bytes_match((uint8_t *)list.data + (item_size * index), (uint8_t *)val, item_size);

    } else {
        /* On failure: list state should remain valid */
        assert(list.item_size == item_size);
        assert(list.alloc == old_alloc);
        /* length should not have increased */
        assert(list.length == old_length);
        /* current_size should not have changed */
        assert(list.current_size == old_current_size);
    }
}
