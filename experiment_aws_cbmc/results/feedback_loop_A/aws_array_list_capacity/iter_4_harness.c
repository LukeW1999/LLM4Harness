#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_capacity_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    size_t max_initial_item_allocation = 10;
    size_t max_item_size = 100;
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, max_item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.alloc >= list.current_size); // Ensure alloc is at least current_size

    /* 2. Call function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* 3. Assert postconditions */
    assert(capacity == list.alloc / list.item_size); // Corrected assertion based on function implementation
    assert(capacity >= list.length);

    /* 4. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
