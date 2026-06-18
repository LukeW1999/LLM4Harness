#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    /* Use the default allocator */
    list.alloc = aws_default_allocator();

    /* Bound the list and allocate its data buffer */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save the original state */
    struct aws_array_list old = list;

    /* Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length is reduced by one; capacity (current_size) stays the same */
        assert(list.length == old.length - 1);
        assert(list.current_size == old.current_size);
    } else {
        /* On failure the list is unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
    }

    /* Fields that must never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
