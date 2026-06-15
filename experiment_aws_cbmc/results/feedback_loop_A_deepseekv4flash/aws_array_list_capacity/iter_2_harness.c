#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness() {
    /* nondet inputs */
    struct aws_array_list list;

    /* assume valid list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* call function */
    size_t capacity = aws_array_list_capacity(&list);

    /* postconditions */
    /* capacity should be current_size / item_size, assuming no division by zero */
    assert(capacity == list.current_size / list.item_size);
    /* list should be unchanged because function takes const pointer */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);
    assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);

    /* validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
