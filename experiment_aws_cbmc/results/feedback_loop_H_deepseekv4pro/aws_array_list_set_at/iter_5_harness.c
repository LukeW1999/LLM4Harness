#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* Zero-initialize to allow ensure_array_list_is_allocated to set fields */
    struct aws_array_list list = {0};
    list.alloc = aws_default_allocator();

    /* Set up a valid array list with bounded capacity and item size */
    ensure_array_list_is_allocated(&list, MAX_INITIAL_ITEM_ALLOCATION);

    /* The list must be valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate a buffer for the value to be written */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Index must be within the current capacity to avoid reallocation */
    size_t index;
    __CPROVER_assume(index < list.current_size);

    /* Save the state before the call */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* Call the function under test */
    int rv = aws_array_list_set_at(&list, val, index);

    /* Postcondition: the array list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Allocator and element size must not change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (rv == AWS_OP_SUCCESS) {
        /* The element at the given index must match val */
        assert(index < list.current_size);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val, list.item_size);

        /* length is updated only if index was beyond the old length */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }
    } else {
        /* On failure, the length must not have changed */
        assert(list.length == old_length);
    }
}
