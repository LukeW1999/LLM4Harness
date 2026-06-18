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
    /* 1. Declare and initialize the array list */
    struct aws_array_list list;

    /* Use bounded initialization to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a val buffer of item_size bytes */
    size_t item_size = list.item_size;
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Non-deterministic index - bound it */
    size_t index;
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_alloc = (void *)list.alloc;
    size_t old_item_size = list.item_size;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant must always hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must never change */
    assert(list.alloc == (struct aws_allocator *)old_alloc);
    assert(list.item_size == old_item_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data pointer must be non-null */
        assert(list.data != NULL);

        /* current_size must accommodate index+1 items */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* length must be at least index+1 */
        assert(list.length >= index + 1);

        /* length update logic */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            assert(list.length == index + 1);
        }

        /* The data at index should be readable */
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + (list.item_size * index), list.item_size));

        /* The data at index should match val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

    } else {
        /* On failure: list state should be unchanged */
        assert(list.item_size == old_item_size);
        assert(list.alloc == (struct aws_allocator *)old_alloc);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
    }
}
