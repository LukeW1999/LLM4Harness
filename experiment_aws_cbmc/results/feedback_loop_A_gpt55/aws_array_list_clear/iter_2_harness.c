#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    if (nondet_bool()) {
        struct aws_allocator *allocator = aws_default_allocator();
        list.alloc = allocator;
    } else {
        list.alloc = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    struct aws_array_list old = list;

    aws_array_list_clear(&list);

    assert(aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, old.current_size);
    assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
}
