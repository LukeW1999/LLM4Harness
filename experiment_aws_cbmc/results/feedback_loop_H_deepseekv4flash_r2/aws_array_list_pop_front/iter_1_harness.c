#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_array_list_pop_front_harness() {
    /* Create a bounded, valid list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Call function */
    int result = aws_array_list_pop_front(&list);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, length must have decreased by 1 */
        assert(list.length == old.length - 1);
        /* Other fields unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* On failure (empty list), list must be unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
