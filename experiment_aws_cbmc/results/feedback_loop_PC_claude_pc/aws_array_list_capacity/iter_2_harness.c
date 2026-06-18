#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_capacity_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* 3. Assert postconditions */
    assert(aws_array_list_is_valid(&list));

    /* If item_size > 0, capacity == current_size / item_size */
    if (list.item_size > 0) {
        assert(capacity == list.current_size / list.item_size);
    } else {
        assert(capacity == 0);
    }
}
