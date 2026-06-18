#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_front_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;

    /* Bounding to limit state space */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    /* Ensure list has at least one element */
    __CPROVER_assume(list.length > 0);

    /* Save old state for postcondition checks */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;

    /* Call the function */
    void *result = aws_array_list_front(&list);

    /* Postconditions */
    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.length == old_length);
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);
    /* The returned pointer should be the first element */
    assert(result == list.data);
}
