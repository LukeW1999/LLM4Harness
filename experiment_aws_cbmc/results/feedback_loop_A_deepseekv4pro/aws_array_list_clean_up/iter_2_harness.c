#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;

    /* Bound the input to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the internal data buffer is allocated (non‑deterministically) so the free path is exercised */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* 1. Changed fields: the entire struct is zeroed out */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* 2. Validity invariant: a zeroed list is still valid for reuse.
     *    aws_array_list_is_valid requires a non-NULL allocator, but clean_up sets alloc to NULL.
     *    Therefore, we only check that the list is zero-initialized, which is the documented postcondition.
     */
}
