#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call function under test */
    size_t length = aws_array_list_length(&list);

    /* 3. Assert postconditions */
    assert(length <= list.current_size / list.item_size);
    assert(length >= 0);

    /* 4. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
