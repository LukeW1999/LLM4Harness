#include <aws/common/array_list.h>
#include <aws/common/math.h>
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

    /* 3. Assert postconditions:
     *    - aws_array_list_clean_up deallocates memory and resets the list (AWS_ZERO_STRUCT)
     *    - After clean_up, all fields should be zeroed out
     */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* 4. Note: After clean_up, the list is zeroed out.
     *    aws_array_list_is_valid requires item_size > 0 OR (data == NULL && current_size == 0 && length == 0)
     *    We only assert the structural postconditions above, not validity,
     *    since item_size == 0 after zeroing may not satisfy is_valid.
     */
}
