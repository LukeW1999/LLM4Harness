#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_capacity_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t item_size;
    size_t initial_item_allocation;
    size_t length;
    size_t current_size;

    /* Bounding assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);  /* Avoid division by zero */

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);

    /* Call function */
    size_t capacity = aws_array_list_capacity(&list);

    /* Postconditions */
    /* Capacity should equal current_size / item_size */
    assert(capacity == list.current_size / list.item_size);

    /* The list should be unchanged */
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);

    /* Validity invariants */
    assert(aws_array_list_is_valid(&list));
}
