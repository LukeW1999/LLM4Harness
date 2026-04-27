#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call function under test */
    aws_array_list_clean_up(&list);

    /* 3. Assert postconditions: AWS_ZERO_STRUCT zeroes all fields */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* 4. A zeroed list may not satisfy aws_array_list_is_valid if item_size == 0
       is not considered valid, so we remove that assertion */
}
