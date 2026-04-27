#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_clean_up_harness(void) {
    /* Allocate and set up the array list */
    struct aws_array_list list;

    /* Use bounded list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the data member is allocated (or NULL) */
    ensure_array_list_has_allocated_data_member(&list);

    /* The list must be valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* === Verify frame conditions and post-conditions === */

    /* After AWS_ZERO_STRUCT(*list), all fields must be zero/NULL */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);
}
