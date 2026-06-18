#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the data member is a valid allocated pointer (or possibly NULL) */
    ensure_array_list_has_allocated_data_member(&list);

    /* Start from a valid state */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* According to the documentation:
     *   "Deallocates any memory that was allocated for this list,
     *    and resets list for reuse or deletion."
     * The implementation zeros the entire structure, so all fields become 0/NULL.
     */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);
}
