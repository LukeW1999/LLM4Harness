#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;

    /* Bound the list to keep state space finite */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save state before call */
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;

    /* Call the function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* Postconditions */
    /* The list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* item_size and current_size must not have changed */
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);

    /* capacity is current_size / item_size when item_size > 0 */
    if (list.item_size > 0) {
        assert(capacity == list.current_size / list.item_size);
    } else {
        assert(capacity == 0);
    }
}
