#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness() {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate data member non-deterministically */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before calling */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 3. Assert postconditions:
     * The function deallocates memory and resets the list (AWS_ZERO_STRUCT).
     * After clean_up, all fields should be zeroed out.
     */

    /* All fields should be zeroed after AWS_ZERO_STRUCT */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);
}
