#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_clear_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    aws_array_list_clear(&list);

    /* After clear, length should be zero and other fields unchanged */
    assert(list.length == 0);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    if (list.current_size > 0) {
        assert_byte_from_buffer_matches(
            (uint8_t *)list.data, list.current_size, &old_bytes);
    }

    assert(aws_array_list_is_valid(&list));
}
