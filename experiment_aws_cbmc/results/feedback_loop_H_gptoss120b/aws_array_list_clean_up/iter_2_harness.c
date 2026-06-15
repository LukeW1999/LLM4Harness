#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_clean_up_harness() {
    /* 1. Declare and bound the data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    aws_array_list_clean_up(&list);

    /* 4. Assert postconditions (fields are cleared as per clean_up) */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 5. No unchanged fields – all fields are expected to be cleared */
    /* (No additional unchanged-field assertions needed) */

    /* Note: After clean_up, aws_array_list_is_valid(&list) is not expected to hold,
       because item_size is set to 0. Hence we do not assert that invariant here. */
}
