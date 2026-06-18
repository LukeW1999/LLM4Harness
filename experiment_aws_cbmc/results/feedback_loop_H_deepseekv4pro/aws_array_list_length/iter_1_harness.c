#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <stddef.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    /* Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Set up a readable value buffer of sufficient size */
    uint8_t val_arr[MAX_ITEM_SIZE];
    const void *val = (const void *)val_arr;

    /* Index can be arbitrary */
    size_t index;
    /* Optionally bound index to limit state space */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION * 2);

    /* Save old state for immutability checks */
    struct aws_array_list old = list;

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Validity invariant always holds after call */
    assert(aws_array_list_is_valid(&list));

    /* Postconditions depending on success or failure */
    if (result == AWS_OP_SUCCESS) {
        /* Core metadata never changes */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Length update: if index was beyond current length, length becomes index+1 */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }

        /* The element at 'index' must contain a copy of 'val' */
        assert_bytes_match((const uint8_t *)val,
                           (const uint8_t *)list.data + index * list.item_size,
                           list.item_size);
    } else {
        /* On failure the list must remain completely unchanged */
        assert(list.data        == old.data);
        assert(list.length      == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size   == old.item_size);
        assert(list.alloc       == old.alloc);
    }
}
