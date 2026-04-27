#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_length_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    __CPROVER_assume(max_initial_item_allocation > 0 && max_initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(max_item_size > 0 && max_item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, max_item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call function under test */
    size_t length = aws_array_list_length(&list);

    /* 3. Assert postconditions */
    assert(length == list.length);
}
