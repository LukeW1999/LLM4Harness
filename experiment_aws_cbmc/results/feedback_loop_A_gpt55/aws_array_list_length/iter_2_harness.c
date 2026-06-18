#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    struct aws_allocator *allocator = aws_default_allocator();
    if (nondet_bool()) {
        list.alloc = allocator;
    } else {
        list.alloc = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);

    size_t length = aws_array_list_length(&list);

    assert(length == old.length);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_byte);
    assert_array_list_equivalence(&list, &old, &old_byte);
    assert(aws_array_list_is_valid(&list));
}
