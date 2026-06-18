#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;

    /* Bound the list to keep the state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate the data member non-deterministically */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 3. Assert postconditions */

    /* All fields must be zeroed after AWS_ZERO_STRUCT(*list) */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* Note: aws_array_list_is_valid on a zeroed struct may require item_size > 0
     * or other invariants. Since the zeroed struct has item_size == 0,
     * the validity check may fail. We only assert the zero postconditions. */
}
