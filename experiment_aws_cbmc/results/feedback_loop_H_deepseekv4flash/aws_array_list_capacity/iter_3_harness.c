#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_capacity_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;

    /* Assumptions for a valid list */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0); /* Avoid division by zero */

    /* Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call function */
    size_t capacity = aws_array_list_capacity(&list);

    /* Postconditions */
    assert(capacity == list.current_size / list.item_size);
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
