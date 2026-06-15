#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    void *val;

    /* Bounded assumptions for the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* Allocate readable memory for val */
    val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* Make the memory readable (already writable from malloc, but treat as readable) */

    /* Prevent overflow in index + 1 */
    __CPROVER_assume(index < SIZE_MAX);

    /* Save old state */
    struct aws_array_list old = list;

    /* Call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postcondition: validity always holds */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Success: element at index must match val */
        uint8_t *elem_ptr = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(elem_ptr, (const uint8_t *)val, list.item_size);

        /* Length update */
        if (index >= old.length) {
            /* length becomes index + 1 (no overflow guaranteed) */
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* data may be reallocated, so we do not assert it unchanged */
    } else {
        /* Failure: list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* Clean up */
    free(val);
}
