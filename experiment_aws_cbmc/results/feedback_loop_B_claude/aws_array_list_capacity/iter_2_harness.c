#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_capacity_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;

    /* 3. Call function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* 4. Assert postconditions */

    /* The list must still be valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* item_size must not change */
    assert(list.item_size == old_item_size);

    /* current_size must not change (read-only operation) */
    assert(list.current_size == old_current_size);

    /* capacity is defined as current_size / item_size */
    if (list.item_size > 0) {
        assert(capacity == list.current_size / list.item_size);
    } else {
        assert(capacity == 0);
    }
}
