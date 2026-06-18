#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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
     */

    /* 3. Assert all fields are zeroed after clean_up */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /*
     * Note: aws_array_list_is_valid is NOT asserted here because after clean_up,
     * item_size is 0 which may violate the validity invariant that requires item_size > 0.
     * The clean_up function intentionally zeros all fields for safe reuse/deletion.
     */
}
