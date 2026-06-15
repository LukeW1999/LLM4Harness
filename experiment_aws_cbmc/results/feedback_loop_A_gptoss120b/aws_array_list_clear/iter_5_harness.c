#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state for later comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_mem = {0};
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_mem);
    }

    /* 3. Call the function under test */
    aws_array_list_clear(&list);

    /* 4. Post‑condition checks */
    /* The list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* Length and current size must be zero after clear */
    assert(list.length == 0);
    assert(list.current_size == 0);

    /* Allocated buffer, alloc, and item_size must not change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Memory contents must be unchanged (clear does not modify the buffer) */
    if (old.current_size > 0) {
        assert_byte_from_buffer_matches(
            (uint8_t *)list.data, old_mem);
    }
}
