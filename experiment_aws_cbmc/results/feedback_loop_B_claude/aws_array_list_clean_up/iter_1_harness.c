#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

/**
 * aws_array_list_clean_up:
 *   - If list->alloc && list->data, releases list->data via aws_mem_release
 *   - Then zeros the entire struct (AWS_ZERO_STRUCT(*list))
 *
 * Postconditions:
 *   - list->alloc == NULL
 *   - list->data == NULL
 *   - list->current_size == 0
 *   - list->length == 0
 *   - list->item_size == 0
 *   - aws_array_list_is_valid(list) holds (zero struct is valid)
 *
 * This function has no return value and no failure path.
 */

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate the data member non-deterministically */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 4. Assert postconditions:
     *    AWS_ZERO_STRUCT zeros all fields unconditionally.
     */

    /* All fields must be zeroed after clean_up */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 5. Validity invariant: a zeroed aws_array_list is valid
     *    (item_size == 0 and data == NULL and current_size == 0 is a valid static/empty list)
     */
    assert(aws_array_list_is_valid(&list));
}
