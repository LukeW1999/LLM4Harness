#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length > 0); // Ensure list is not empty

    /* 2. Call function under test */
    void *front_item = aws_array_list_front(&list);

    /* 3. Assert postconditions */
    assert(front_item != NULL); // Front item should not be NULL if list is not empty
    assert(front_item == list.data); // Front item should point to the first element of the list
}
