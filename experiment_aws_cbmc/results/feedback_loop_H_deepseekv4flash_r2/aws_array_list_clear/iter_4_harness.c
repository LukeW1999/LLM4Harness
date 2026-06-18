#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_clear_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;

    /* Bound the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* Call function */
    aws_array_list_clear(&list);

    /* Postconditions */
    assert(list.length == 0);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
