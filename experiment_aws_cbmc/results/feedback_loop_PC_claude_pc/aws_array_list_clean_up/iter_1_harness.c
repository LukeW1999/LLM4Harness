#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Harness for aws_array_list_clean_up
 *
 * From the Doxygen:
 *   "Deallocates any memory that was allocated for this list, and resets list for reuse or deletion."
 *
 * From the implementation:
 *   if (list->alloc && list->data) { aws_mem_release(list->alloc, list->data); }
 *   AWS_ZERO_STRUCT(*list);
 *
 * Postconditions:
 *   1. After the call, all fields of *list are zeroed (AWS_ZERO_STRUCT).
 *      - list->alloc == NULL
 *      - list->current_size == 0
 *      - list->length == 0
 *      - list->item_size == 0
 *      - list->data == NULL
 *   2. The function is void — no return value to check.
 *   3. aws_array_list_is_valid must hold after the call (zeroed list is valid).
 */

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;

    /* Bound the list to keep the state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate the data member non-deterministically */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state (for frame conditions — though everything changes here) */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 4. Assert postconditions */

    /* All fields must be zeroed after AWS_ZERO_STRUCT(*list) */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* 5. Assert validity invariant holds after the call */
    /* A zeroed list: data == NULL, current_size == 0, length == 0, item_size == 0
     * aws_array_list_is_valid should accept this state */
    assert(aws_array_list_is_valid(&list));
}
