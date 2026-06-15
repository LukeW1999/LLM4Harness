#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_clear_harness(void) {
    /* 1. Declare the list and bound its fields */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 2. Allocate the internal buffer and make the list valid */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 3. Make the current size nondeterministic but within bounds */
    size_t max_sz = list.length;
    __CPROVER_assume(list.current_size <= max_sz);
    __CPROVER_assume(list.current_size == 0 || list.current_size > 0);

    /* 4. Save old state for later comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_mem = {0};
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_mem);
    }

    /* 5. Call the function under test */
    aws_array_list_clear(&list);

    /* 6. Post‑condition checks */
    assert(aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (old.current_size > 0) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, old_mem);
    }
}
