#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call the function under test */
    aws_array_list_clean_up(&list);

    /*
     * Postconditions from Doxygen:
     * "Deallocates any memory that was allocated for this list, and resets list for reuse or deletion."
     *
     * The implementation calls AWS_ZERO_STRUCT(*list), which zeroes all fields:
     *   - alloc  -> NULL
     *   - current_size -> 0
     *   - length -> 0
     *   - item_size -> 0
     *   - data -> NULL
     *
     * After clean_up, the list should be zeroed out.
     */

    /* 3. Assert all fields are zeroed (reset) */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /*
     * Note: After zeroing, aws_array_list_is_valid may not hold because
     * item_size == 0 might violate the validity predicate.
     * We remove that assertion.
     */
}
